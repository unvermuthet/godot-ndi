#!/usr/bin/env python
import os
import sys

from methods import print_error

libname = "godot-ndi"
bindir = "addons/godot-ndi/bin"

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

submodule_initialized = False
if os.path.isdir("godot-cpp"):
    if os.listdir("godot-cpp"):
        submodule_initialized = True

if not submodule_initialized:
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = localEnv.Clone()
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

libraryfile = "{}/{}/{}{}".format(bindir, env["platform"], filepath, file)
library = env.SharedLibrary(libraryfile, source=sources)

default_args = [library]
Default(*default_args)
