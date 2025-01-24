#!/usr/bin/env python
import os
import sys

from methods import print_error

libname = "godot-ndi"

# This is nice for development, since this whole folder is .gdignored
# installdir = "../godot-ndi-bin"
installdir = False

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

submodule_initialized = False
if os.path.isdir("godot-cpp"):
    if os.listdir("godot-cpp"):
        submodule_initialized = True

if not submodule_initialized:
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/", "ndi/"])
sources = Glob("src/*.cpp")

if env["platform"] == "windows":
    env.Append(LIBS=["ws2_32"])

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])
filepath = ""

if env["platform"] == "macos" or env["platform"] == "ios":
    filepath = "{}.framework/".format(env["platform"])
    file = "{}.{}.{}".format(libname, env["platform"], env["target"])

libraryfile = "bin/{}/{}{}".format(env["platform"], filepath, file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

default_args = [library]

if installdir:
    copy = env.InstallAs("{}/{}/{}lib{}".format(installdir, env["platform"], filepath, file), library)
    default_args = [library, copy]

Default(*default_args)
