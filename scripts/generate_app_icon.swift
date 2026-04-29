#!/usr/bin/env swift

import AppKit
import Foundation

let fileManager = FileManager.default
let root = URL(fileURLWithPath: fileManager.currentDirectoryPath, isDirectory: true)
let packaging = root.appendingPathComponent("packaging", isDirectory: true)
let scratch = URL(fileURLWithPath: NSTemporaryDirectory(), isDirectory: true)
    .appendingPathComponent("FieldOfChaosAppIcon-\(UUID().uuidString)", isDirectory: true)
let iconset = scratch.appendingPathComponent("AppIcon.iconset", isDirectory: true)
let icns = packaging.appendingPathComponent("AppIcon.icns")

try fileManager.createDirectory(at: iconset, withIntermediateDirectories: true)
defer { try? fileManager.removeItem(at: scratch) }

let files: [(Int, String)] = [
    (16, "icon_16x16.png"),
    (32, "icon_16x16@2x.png"),
    (32, "icon_32x32.png"),
    (64, "icon_32x32@2x.png"),
    (128, "icon_128x128.png"),
    (256, "icon_128x128@2x.png"),
    (256, "icon_256x256.png"),
    (512, "icon_256x256@2x.png"),
    (512, "icon_512x512.png"),
    (1024, "icon_512x512@2x.png")
]

for (size, filename) in files {
    let image = makeIcon(size: size)
    let url = iconset.appendingPathComponent(filename)
    try writePNG(image, to: url)
}

let task = Process()
task.executableURL = URL(fileURLWithPath: "/usr/bin/iconutil")
task.arguments = ["-c", "icns", iconset.path, "-o", icns.path]
try task.run()
task.waitUntilExit()

guard task.terminationStatus == 0 else {
    fputs("iconutil failed\n", stderr)
    exit(Int32(task.terminationStatus))
}

print("Generated \(icns.path)")

func makeIcon(size: Int) -> NSImage {
    let side = CGFloat(size)
    let image = NSImage(size: NSSize(width: side, height: side))
    image.lockFocus()

    let rect = NSRect(x: 0, y: 0, width: side, height: side)
    let corner = side * 0.18
    let base = NSBezierPath(roundedRect: rect.insetBy(dx: side * 0.035, dy: side * 0.035), xRadius: corner, yRadius: corner)
    NSGradient(
        starting: NSColor(calibratedRed: 0.05, green: 0.10, blue: 0.09, alpha: 1),
        ending: NSColor(calibratedRed: 0.20, green: 0.30, blue: 0.24, alpha: 1)
    )?.draw(in: base, angle: -35)

    NSColor(calibratedRed: 0.84, green: 0.66, blue: 0.28, alpha: 1).setStroke()
    base.lineWidth = max(2, side * 0.035)
    base.stroke()

    NSColor(calibratedWhite: 1, alpha: 0.11).setStroke()
    for index in 1..<4 {
        let offset = side * CGFloat(index) / 4
        let vertical = NSBezierPath()
        vertical.move(to: NSPoint(x: offset, y: side * 0.18))
        vertical.line(to: NSPoint(x: offset, y: side * 0.82))
        vertical.lineWidth = max(1, side * 0.006)
        vertical.stroke()

        let horizontal = NSBezierPath()
        horizontal.move(to: NSPoint(x: side * 0.18, y: offset))
        horizontal.line(to: NSPoint(x: side * 0.82, y: offset))
        horizontal.lineWidth = max(1, side * 0.006)
        horizontal.stroke()
    }

    NSColor(calibratedRed: 0.90, green: 0.32, blue: 0.25, alpha: 1).setFill()
    NSBezierPath(ovalIn: NSRect(x: side * 0.62, y: side * 0.58, width: side * 0.16, height: side * 0.16)).fill()

    NSColor(calibratedRed: 0.28, green: 0.62, blue: 0.88, alpha: 1).setFill()
    NSBezierPath(ovalIn: NSRect(x: side * 0.24, y: side * 0.28, width: side * 0.18, height: side * 0.18)).fill()

    NSColor(calibratedRed: 0.84, green: 0.66, blue: 0.28, alpha: 1).setStroke()
    let scope = NSBezierPath()
    scope.appendOval(in: NSRect(x: side * 0.31, y: side * 0.36, width: side * 0.38, height: side * 0.38))
    scope.move(to: NSPoint(x: side * 0.50, y: side * 0.31))
    scope.line(to: NSPoint(x: side * 0.50, y: side * 0.79))
    scope.move(to: NSPoint(x: side * 0.26, y: side * 0.55))
    scope.line(to: NSPoint(x: side * 0.74, y: side * 0.55))
    scope.lineWidth = max(2, side * 0.035)
    scope.stroke()

    let text = "FoC" as NSString
    let font = NSFont.systemFont(ofSize: side * 0.15, weight: .bold)
    let paragraph = NSMutableParagraphStyle()
    paragraph.alignment = .center
    let attributes: [NSAttributedString.Key: Any] = [
        .font: font,
        .foregroundColor: NSColor.white,
        .paragraphStyle: paragraph
    ]
    text.draw(in: NSRect(x: 0, y: side * 0.11, width: side, height: side * 0.2), withAttributes: attributes)

    image.unlockFocus()
    return image
}

func writePNG(_ image: NSImage, to url: URL) throws {
    guard let tiff = image.tiffRepresentation,
          let bitmap = NSBitmapImageRep(data: tiff),
          let data = bitmap.representation(using: .png, properties: [:]) else {
        throw NSError(domain: "FieldOfChaosIcon", code: 1)
    }
    try data.write(to: url, options: .atomic)
}
