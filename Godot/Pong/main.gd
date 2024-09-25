extends Node2D

var player_score = 0
var opponent_score = 0

func _ready():
	$Ball.connect("score", Callable(self, "_on_score"))

func _on_score(player):
	if player == "player":
		player_score += 1
	else:
		opponent_score += 1
		
	$PlayerScore.text = str(player_score)
	$OpponentScore.text = str(opponent_score)
	$Ball.reset()
