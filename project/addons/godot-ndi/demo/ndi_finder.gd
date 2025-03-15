extends NDIFinder

@onready var player: VideoStreamPlayer = $"../VideoStreamPlayer"

func _on_sources_changed(sources: Array) -> void:
	player.stream = get_sources().pop_front()
	player.play()

func _on_sources_found(found_sources: Array) -> void:
	print("found: ", found_sources.pop_front().get_name())

func _on_sources_gone(gone_sources: Array) -> void:
	print("gone: ", gone_sources.pop_front().get_name())
