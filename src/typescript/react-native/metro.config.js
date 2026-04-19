const path = require("path");
const { getDefaultConfig } = require("@expo/metro-config");

const projectRoot = __dirname;
const workspaceRoot = path.resolve(projectRoot, "..", "..", "..");

const config = getDefaultConfig(projectRoot);

// Watch the monorepo so Metro can resolve the local @banana/ui file dependency.
config.watchFolders = [workspaceRoot];
config.resolver.nodeModulesPaths = [
    path.resolve(projectRoot, "node_modules"),
    path.resolve(workspaceRoot, "node_modules")
];
config.resolver.disableHierarchicalLookup = true;
config.resolver.unstable_enableSymlinks = true;
config.resolver.unstable_enablePackageExports = true;

module.exports = config;
