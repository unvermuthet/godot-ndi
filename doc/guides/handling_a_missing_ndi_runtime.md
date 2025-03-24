# Handling a missing NDI Runtime

**This extensions depends on the NDI Runtime being present on the system running Godot.**

If it is missing, the extension will not register any of the NDI-dependant classes on startup. When trying to open or run a scene which already had these Nodes and Resources added, they will be temporarily replaced with [MissingNode](https://docs.godotengine.org/en/stable/classes/class_missingnode.html) and [MissingResource](https://docs.godotengine.org/en/stable/classes/class_missingresource.html).

The Godot Editor will automatically show [NDIRuntimeDialog](class_NDIRuntimeDialog) if the NDI Runtime is missing. To also warn the users of _your_ application, just add this Node to your scene or create your own implementation with its static methods. It should be noted that this won't stop a invalid access on a [MissingNode](https://docs.godotengine.org/en/stable/classes/class_missingnode.html) or [MissingResource](https://docs.godotengine.org/en/stable/classes/class_missingresource.html). To safe-guard against this you may make use of [is_runtime_loaded](class_NDIRuntimeDialog_method_is_runtime_loaded).
