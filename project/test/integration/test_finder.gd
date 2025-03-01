extends GutTest

func test_finds_grouped_source():
	var output: NDIOutput = add_child_autoqfree(NDIOutput.new())
	output.name = "GodotSource"
	output.groups = ["GodotGroup"]

	var finder: NDIFinder = add_child_autoqfree(NDIFinder.new())
	finder.show_local_sources = true
	finder.groups = ["GodotGroup"]

	await wait_for_signal(finder.sources_changed, 10, "Waiting for source to be found")

	assert_eq(finder.get_sources().size(), 1, "One source found")
	assert_string_ends_with(finder.get_sources()[0].name, " (GodotSource (Game))", "Correct source found")