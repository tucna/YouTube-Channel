extends CharacterBody2D

signal score(player)

@export var initial_speed = 400

var speed = initial_speed
var direction = Vector2.ZERO
var screen_size

func _ready():
	screen_size = get_viewport_rect().size
	screen_size.y -= $CollisionShape2D.shape.size.y
	reset()

func reset():
	position = Vector2(576, 340)
	direction = Vector2(randf_range(-1, 1), randf_range(-0.8, 0.8)).normalized()
	speed = initial_speed

func _physics_process(delta):
	var collision = move_and_collide(direction * speed * delta)
	if collision:
		direction = direction.bounce(collision.get_normal())
		speed += 25
		
	if position.y <= 0 or position.y >= screen_size.y:
		direction.y = -direction.y
		position.y = clamp(position.y, 0, screen_size.y)


func _on_visible_on_screen_notifier_2d_screen_exited() -> void:
	if position.x < 0:
		emit_signal("score", "opponent")
	else:
		emit_signal("score", "player")
