extends NDIFinder

@onready var player: VideoStreamPlayer = $"../VideoStreamPlayer"

func _on_sources_changed() -> void:
	player.stream = get_sources().pop_front()
	player.play()

func _on_sources_found(found_sources: Array[VideoStreamNDI]) -> void:
	print("NDI sources found: ", found_sources.map(func (source: VideoStreamNDI): return source.get_name()))

func _on_sources_gone(gone_sources: Array[VideoStreamNDI]) -> void:
	print("NDI sources gone: ", gone_sources.map(func (source: VideoStreamNDI): return source.get_name()))
