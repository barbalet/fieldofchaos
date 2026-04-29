// swift-tools-version: 5.9

import PackageDescription

let package = Package(
    name: "FieldOfChaos",
    platforms: [
        .macOS(.v13)
    ],
    products: [
        .library(name: "FieldOfChaosEngine", targets: ["FieldOfChaosEngine"]),
        .executable(name: "FieldOfChaosAppShell", targets: ["FieldOfChaosAppShell"])
    ],
    targets: [
        .target(
            name: "FieldOfChaosEngine",
            path: "src/engine",
            exclude: ["fieldofchaos_engine.o"],
            publicHeadersPath: "include",
            cSettings: [
                .headerSearchPath("include")
            ]
        ),
        .executableTarget(
            name: "FieldOfChaosAppShell",
            dependencies: ["FieldOfChaosEngine"],
            path: "AppShell",
            resources: [
                .process("Shaders")
            ]
        )
    ]
)
