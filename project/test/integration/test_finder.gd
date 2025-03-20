extends GutTest

func test_finds_grouped_source():
	var finder: NDIFinder = add_child_autoqfree(NDIFinder.new())
	finder.show_local_sources = true
	finder.groups = ["GodotGroup"]
	watch_signals(finder)

	var output: NDIOutput = add_child_autoqfree(NDIOutput.new())
	output.name = "GodotSource"
	output.groups = ["GodotGroup"]

	await wait_for_signal(finder.sources_changed, 10, "Waiting for sources to change")

	assert_signal_emit_count(finder, "sources_changed", 1, "Sources have changed once")
	assert_signal_emit_count(finder, "sources_found", 1, "New sources have been found once")
	assert_signal_emit_count(finder, "sources_gone", 0, "Existing sources have never disappeared")

	assert_eq(finder.get_sources().size(), 1, "There is one source in GodotGroup")
	assert_string_ends_with(finder.get_sources()[0].name, " (GodotSource)", "Correct source found")

	# Now change the name of the NDIOutput
	output.name = "NewGodotSource"

	await wait_for_signal(finder.sources_changed, 10, "Waiting for sources to change")

	assert_signal_emit_count(finder, "sources_changed", 2, "Sources have changed twice")
	assert_signal_emit_count(finder, "sources_found", 2, "New source has also been found")
	assert_signal_emit_count(finder, "sources_gone", 1, "Source with old name is gone")

	assert_eq(finder.get_sources().size(), 1, "There is still one source in GodotGroup")
	assert_string_ends_with(finder.get_sources()[0].name, " (NewGodotSource)", "Correct new source found")
