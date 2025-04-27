#!/usr/bin/env python
import os
import sys
from SCons.Script import Environment, SConscript, Default, Glob, Variables, PathVariable, Import, Copy

import version

extensionName = "godot-ndi"

try:
    Import("env")
except Exception:
    env_gcpp = Environment()

env_gcpp["build_profile"] = env_gcpp.File("build_profile.json").path

opts = Variables()

opts.Add(PathVariable(
    key="install_dir",
    help="Directory to copy the addon folder into",
    default=env_gcpp.get("install_dir", None),
    validator=PathVariable.PathIsDirCreate
))

opts.Update(env_gcpp)
env_gcpp.Help(opts.GenerateHelpText(env_gcpp))

# Check if the godot-cpp submodule is initialized
if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

# Consume the godot-cpp environment
env_gcpp = SConscript("godot-cpp/SConstruct", {"env": env_gcpp})
env = env_gcpp.Clone()

# Sources
env.Append(CPPPATH=["src/", "ndi/"])
sources = Glob("src/*.cpp")

# Git commit hash and tag
env.Append(CPPDEFINES={
    "GIT_COMMIT_HASH": f'\\"{version.commit_hash}\\"',
    "GIT_COMMIT_TAG": f'\\"{version.commit_tag}\\"'
})

# Add docs
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# Include Windows SDK
if env["platform"] == "windows":
    env.Append(LIBS=["ws2_32"])

# Disable deprecated warnings
if env["platform"] != "windows" or env["use_mingw"]:
    env.Append(CCFLAGS=["-Wno-deprecated-declarations"])

# Ensure consistency between tool-chains
env["SHLIBPREFIX"] = "lib"
if env["platform"] == "macos":
    env["SHLIBSUFFIX"] = ".dylib"

# Output shared library
addonFolder = f"project/addons/{extensionName}"
libraryFile = f"{extensionName}{env['suffix']}{env['SHLIBSUFFIX']}"
libraryPath = f"{addonFolder}/bin/{env['platform']}/{libraryFile}"
library = env.SharedLibrary(libraryPath, source=sources)

# Copy the addonFolder to install_in
if env.get("install_dir", None):
    install = env.Install(
        env["install_dir"],
        addonFolder,
    )
    Default([library, install])
else:
    Default([library])
