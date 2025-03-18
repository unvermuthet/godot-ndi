extends VideoStreamPlayer

@export var playlist: Array[VideoStream] = []

func _ready() -> void:
	_on_finished()
	
func _on_finished() -> void:
	if playlist.is_empty():
		return
	
	var next_idx = playlist.find(stream) + 1
	if next_idx >= playlist.size():
		next_idx = 0
	
	stream = playlist[next_idx]
	play()
