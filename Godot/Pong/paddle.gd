extends CharacterBody2D

@export var speed = 400
@export var is_player = true

var screen_size

func _ready():
	screen_size = get_viewport_rect().size
	screen_size.y -= $CollisionShape2D.shape.size.y

func _physics_process(delta):
	var movement = Vector2.ZERO
	if is_player:
		if Input.is_action_pressed("ui_up"):
			movement.y -= 1
		if Input.is_action_pressed("ui_down"):
			movement.y += 1
			
	move_and_collide(movement * speed * delta)
	
	# Clamp the paddle's position to stay within the screen
	position.y = clamp(position.y, 0, screen_size.y)
