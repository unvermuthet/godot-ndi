extends NDIFinder

@onready var player: VideoStreamPlayer = $"../VideoStreamPlayer"

func _on_sources_changed() -> void:
	player.stream = get_sources().pop_front()
	player.play()
