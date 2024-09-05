import pygame
import random

# Initialize Pygame
pygame.init()

screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("My Super Game")

# Main Loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        # Fill the screen with white
        screen.fill((255,255,255))

        # Update the display
        pygame.display.flip()

# Quit Pygame
pygame.quit()