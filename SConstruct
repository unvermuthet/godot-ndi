#!/usr/bin/env python
import os
import sys
import subprocess

from methods import print_error

libname = "godot-ndi"
bindir = f"project/addons/{libname}/bin"

localEnv = Environment(tools=["default"], PLATFORM="")
localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

# Check if the godot-cpp submodule is initialized

submodule_initialized = False
if os.path.isdir("godot-cpp"):
    if os.listdir("godot-cpp"):
        submodule_initialized = True

if not submodule_initialized:
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

# Extend the godot-cpp environment
env = localEnv.Clone()
env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

# Get commit hash and version tag

commit_hash = ""
commit_tag = ""

try:
    commit_hash = subprocess.check_output(
        ["git", "rev-parse", "HEAD"]
    ).strip().decode("utf-8")
except subprocess.CalledProcessError:
    print("Failed to get the latest commit hash.")

try:
    commit_tag = subprocess.check_output(
        ["git", "describe", "--tags", "--exact-match"]
    ).strip().decode("utf-8")
except subprocess.CalledProcessError:
    print("No tag found for the current commit.")

# Sources
env.Append(CPPPATH=["src/", "ndi/"])
sources = Glob(
    "src/*.cpp",
    exclude=["src/initialize.cpp", "src/ndi_version_check.cpp"]
)

# These are included seperately to allow for the commit hash and
# version tag to be included without invalidating cache for every file
sources += env.SharedObject(["src/initialize.cpp", "src/ndi_version_check.cpp"], CPPDEFINES={
    "GIT_COMMIT_HASH": f'\\"{commit_hash}\\"', "GIT_COMMIT_TAG": f'\\"{commit_tag}\\"'
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

# Set paths
file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])
libraryfile = "{}/{}/{}".format(bindir, env["platform"], file)

Default(env.SharedLibrary(libraryfile, source=sources))
