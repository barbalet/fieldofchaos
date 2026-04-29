import MetalKit
import SwiftUI

struct BoardVertex {
    var position: SIMD2<Float>
    var color: SIMD4<Float>
}

final class BoardRenderer: NSObject, MTKViewDelegate {
    private let device: MTLDevice
    private let commandQueue: MTLCommandQueue
    private let pipeline: MTLRenderPipelineState
    private var state = BoardRenderState(columns: 16, rows: 10, moveCells: [], targetCells: [], selectedCell: nil, objectiveCell: nil, coverCells: [], actors: [])

    init?(view: MTKView) {
        guard let device = view.device ?? MTLCreateSystemDefaultDevice(),
              let commandQueue = device.makeCommandQueue() else {
            return nil
        }
        self.device = device
        self.commandQueue = commandQueue

        do {
            let library: MTLLibrary
            do {
                library = try device.makeDefaultLibrary(bundle: Self.shaderBundle)
            } catch {
                library = try device.makeLibrary(source: Self.fallbackShaderSource, options: nil)
            }
            let descriptor = MTLRenderPipelineDescriptor()
            descriptor.vertexFunction = library.makeFunction(name: "board_vertex")
            descriptor.fragmentFunction = library.makeFunction(name: "board_fragment")
            descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat
            pipeline = try device.makeRenderPipelineState(descriptor: descriptor)
        } catch {
            return nil
        }

        super.init()
        view.device = device
        view.clearColor = MTLClearColor(red: 0.065, green: 0.075, blue: 0.07, alpha: 1)
    }

    private static var shaderBundle: Bundle {
        #if SWIFT_PACKAGE
        return .module
        #else
        return .main
        #endif
    }

    private static let fallbackShaderSource = """
    #include <metal_stdlib>
    using namespace metal;
    struct BoardVertex { float2 position; float4 color; };
    struct RasterVertex { float4 position [[position]]; float4 color; };
    vertex RasterVertex board_vertex(const device BoardVertex *vertices [[buffer(0)]], uint vertex_id [[vertex_id]]) {
        RasterVertex out;
        out.position = float4(vertices[vertex_id].position, 0.0, 1.0);
        out.color = vertices[vertex_id].color;
        return out;
    }
    fragment float4 board_fragment(RasterVertex in [[stage_in]]) {
        return in.color;
    }
    """

    func update(state: BoardRenderState) {
        self.state = state
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }

    func draw(in view: MTKView) {
        guard let drawable = view.currentDrawable,
              let passDescriptor = view.currentRenderPassDescriptor,
              let commandBuffer = commandQueue.makeCommandBuffer(),
              let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: passDescriptor) else {
            return
        }

        var vertices = makeVertices()
        encoder.setRenderPipelineState(pipeline)
        encoder.setVertexBytes(&vertices, length: MemoryLayout<BoardVertex>.stride * vertices.count, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertices.count)
        encoder.endEncoding()
        commandBuffer.present(drawable)
        commandBuffer.commit()
    }

    private func makeVertices() -> [BoardVertex] {
        var vertices: [BoardVertex] = []
        let cols = max(1, state.columns)
        let rows = max(1, state.rows)
        let cellWidth = 2.0 / Float(cols)
        let cellHeight = 2.0 / Float(rows)

        func rect(_ x0: Float, _ y0: Float, _ x1: Float, _ y1: Float, _ color: SIMD4<Float>) {
            vertices.append(contentsOf: [
                BoardVertex(position: SIMD2(x0, y0), color: color),
                BoardVertex(position: SIMD2(x1, y0), color: color),
                BoardVertex(position: SIMD2(x0, y1), color: color),
                BoardVertex(position: SIMD2(x1, y0), color: color),
                BoardVertex(position: SIMD2(x1, y1), color: color),
                BoardVertex(position: SIMD2(x0, y1), color: color)
            ])
        }

        func cellRect(x: Int, y: Int, inset: Float, color: SIMD4<Float>) {
            let x0 = -1 + Float(x) * cellWidth + inset
            let x1 = -1 + Float(x + 1) * cellWidth - inset
            let y0 = 1 - Float(y + 1) * cellHeight + inset
            let y1 = 1 - Float(y) * cellHeight - inset
            rect(x0, y0, x1, y1, color)
        }

        for x in 0..<cols {
            for y in 0..<rows {
                let base = ((x + y) % 2 == 0) ? SIMD4<Float>(0.12, 0.17, 0.14, 1) : SIMD4<Float>(0.10, 0.145, 0.12, 1)
                cellRect(x: x, y: y, inset: 0.004, color: base)
            }
        }

        for cell in state.moveCells {
            cellRect(x: cell.x, y: cell.y, inset: 0.012, color: SIMD4<Float>(0.16, 0.36, 0.26, 0.88))
        }

        for cell in state.coverCells {
            cellRect(x: cell.x, y: cell.y, inset: 0.014, color: SIMD4<Float>(0.21, 0.22, 0.20, 0.96))
        }

        for cell in state.targetCells {
            cellRect(x: cell.x, y: cell.y, inset: 0.012, color: SIMD4<Float>(0.43, 0.27, 0.18, 0.92))
        }

        if let objective = state.objectiveCell {
            cellRect(x: objective.x, y: objective.y, inset: 0.02, color: SIMD4<Float>(0.36, 0.31, 0.58, 0.95))
        }

        if let selected = state.selectedCell {
            cellRect(x: selected.x, y: selected.y, inset: 0.018, color: SIMD4<Float>(0.78, 0.72, 0.36, 0.95))
        }

        for actor in state.actors {
            let color = actor.isSelected
                ? SIMD4<Float>(0.96, 0.88, 0.38, 1)
                : actor.isPlayer ? SIMD4<Float>(0.22, 0.56, 0.86, 1) : SIMD4<Float>(0.78, 0.25, 0.24, 1)
            cellRect(x: actor.x, y: actor.y, inset: min(cellWidth, cellHeight) * 0.18, color: color)
        }

        return vertices
    }
}

struct SkirmishMetalView: NSViewRepresentable {
    var renderState: BoardRenderState

    func makeCoordinator() -> Coordinator {
        Coordinator()
    }

    func makeNSView(context: Context) -> MTKView {
        let view = MTKView()
        view.enableSetNeedsDisplay = true
        view.isPaused = true
        view.framebufferOnly = true
        context.coordinator.renderer = BoardRenderer(view: view)
        view.delegate = context.coordinator.renderer
        return view
    }

    func updateNSView(_ view: MTKView, context: Context) {
        context.coordinator.renderer?.update(state: renderState)
        view.setNeedsDisplay(view.bounds)
    }

    final class Coordinator {
        var renderer: BoardRenderer?
    }
}
