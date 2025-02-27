extends GutTest

func test_finds_public_source():
	var output: NDIOutput = add_child_autofree(NDIOutput.new())
	output.name = "Godot Test"

	var finder: NDIFinder = add_child_autofree(NDIFinder.new())
	await wait_for_signal(finder.sources_changed, 5, "Waiting for source to be found")

	assert_eq(finder.get_sources().size(), 1, "One source found")
	assert_string_contains(finder.get_sources()[0].name, "Godot Test (Game)", "Correct source found")
