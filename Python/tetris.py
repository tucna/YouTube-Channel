import pygame
import random

# Initialize Pygame
pygame.init()

# Define colors using RGB tuples
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
CYAN = (0, 255, 255)
YELLOW = (255, 255, 0)
MAGENTA = (255, 0, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
ORANGE = (255, 165, 0)
GRAY = (128, 128, 128)  # Color for the border

# Game dimensions
BLOCK_SIZE = 30  # Size of each tetromino block in pixels
GRID_WIDTH = 10  # Number of columns in the game grid
GRID_HEIGHT = 20  # Number of rows in the game grid
BORDER_WIDTH = 4  # Width of the border around the game area in pixels
SCREEN_WIDTH = BLOCK_SIZE * GRID_WIDTH + BORDER_WIDTH * 2 + 200  # Total screen width, including space for score
SCREEN_HEIGHT = BLOCK_SIZE * GRID_HEIGHT + BORDER_WIDTH  # Total screen height

# Define tetromino shapes using 2D lists
# Each sublist represents a row, and 1 indicates a filled block
SHAPES = [
    [[1, 1, 1, 1]],  # I-shape
    [[1, 1], [1, 1]],  # O-shape
    [[1, 1, 1], [0, 1, 0]],  # T-shape
    [[1, 1, 1], [1, 0, 0]],  # L-shape
    [[1, 1, 1], [0, 0, 1]],  # J-shape
    [[1, 1, 0], [0, 1, 1]],  # S-shape
    [[0, 1, 1], [1, 1, 0]]  # Z-shape
]

# Colors for each tetromino shape
COLORS = [CYAN, YELLOW, MAGENTA, RED, GREEN, BLUE, ORANGE]

class Tetris:
    def __init__(self):
        # Set up the game window
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Tetris")
        
        # Create a clock object to control the game's framerate
        self.clock = pygame.time.Clock()
        
        # Initialize the game grid (0 represents empty cells)
        self.grid = [[0 for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]
        
        # Create the first tetromino piece
        self.current_piece = self.new_piece()
        
        # Game state variables
        self.game_over = False
        self.score = 0
        
        # Set up font for rendering text
        self.font = pygame.font.Font(None, 36)
        
        # Set up delay for continuous movement
        self.move_delay = 100  # Delay in milliseconds
        self.last_move_time = {pygame.K_LEFT: 0, pygame.K_RIGHT: 0, pygame.K_DOWN: 0}

    def new_piece(self):
        # Randomly select a shape and its corresponding color
        shape = random.choice(SHAPES)
        color = COLORS[SHAPES.index(shape)]
        
        # Return a dictionary representing the new piece
        return {
            'shape': shape,
            'color': color,
            'x': GRID_WIDTH // 2 - len(shape[0]) // 2,  # Center the piece horizontally
            'y': 0  # Start at the top of the grid
        }

    def valid_move(self, piece, x, y):
        # Check if the piece can be placed at the given position
        for i, row in enumerate(piece['shape']):
            for j, cell in enumerate(row):
                if cell:
                    if (x + j < 0 or x + j >= GRID_WIDTH or  # Check horizontal boundaries
                        y + i >= GRID_HEIGHT or  # Check bottom boundary
                        (y + i >= 0 and self.grid[y + i][x + j])):  # Check collision with placed pieces
                        return False
        return True

    def place_piece(self, piece):
        # Place the piece on the grid
        for i, row in enumerate(piece['shape']):
            for j, cell in enumerate(row):
                if cell:
                    self.grid[piece['y'] + i][piece['x'] + j] = piece['color']

    def remove_full_rows(self):
        # Identify and remove full rows, then add new empty rows at the top
        full_rows = [i for i, row in enumerate(self.grid) if all(row)]
        for row in full_rows:
            del self.grid[row]
            self.grid.insert(0, [0 for _ in range(GRID_WIDTH)])
        return len(full_rows)  # Return the number of rows removed

    def rotate_piece(self, piece):
        # Rotate the piece 90 degrees clockwise
        return {
            'shape': list(zip(*reversed(piece['shape']))),  # Transpose and reverse the shape matrix
            'color': piece['color'],
            'x': piece['x'],
            'y': piece['y']
        }

    def draw_border(self):
        # Draw the border around the game area
        pygame.draw.rect(self.screen, GRAY, (0, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT), BORDER_WIDTH)

    def draw(self):
        # Clear the screen
        self.screen.fill(BLACK)
        
        # Draw the border
        self.draw_border()

        # Draw the placed pieces on the grid
        for y, row in enumerate(self.grid):
            for x, color in enumerate(row):
                if color:
                    pygame.draw.rect(self.screen, color, 
                                     (x * BLOCK_SIZE + BORDER_WIDTH, 
                                      y * BLOCK_SIZE, 
                                      BLOCK_SIZE - 1, BLOCK_SIZE - 1))

        # Draw the current falling piece
        for i, row in enumerate(self.current_piece['shape']):
            for j, cell in enumerate(row):
                if cell:
                    pygame.draw.rect(self.screen, self.current_piece['color'],
                                     ((self.current_piece['x'] + j) * BLOCK_SIZE + BORDER_WIDTH,
                                      (self.current_piece['y'] + i) * BLOCK_SIZE,
                                      BLOCK_SIZE - 1, BLOCK_SIZE - 1))

        # Draw the score
        score_text = self.font.render(f"Score: {self.score}", True, WHITE)
        self.screen.blit(score_text, (SCREEN_WIDTH - 190, 10))

        # Draw game over message if the game has ended
        if self.game_over:
            game_over_text = self.font.render("GAME OVER", True, WHITE)
            self.screen.blit(game_over_text, (SCREEN_WIDTH // 2 - 70, SCREEN_HEIGHT // 2))

        # Update the display
        pygame.display.flip()

    def handle_continuous_movement(self):
        # Handle continuous key presses for smoother movement
        keys = pygame.key.get_pressed()
        current_time = pygame.time.get_ticks()

        # Move left
        if keys[pygame.K_LEFT] and current_time - self.last_move_time[pygame.K_LEFT] > self.move_delay:
            if self.valid_move(self.current_piece, self.current_piece['x'] - 1, self.current_piece['y']):
                self.current_piece['x'] -= 1
                self.last_move_time[pygame.K_LEFT] = current_time

        # Move right
        if keys[pygame.K_RIGHT] and current_time - self.last_move_time[pygame.K_RIGHT] > self.move_delay:
            if self.valid_move(self.current_piece, self.current_piece['x'] + 1, self.current_piece['y']):
                self.current_piece['x'] += 1
                self.last_move_time[pygame.K_RIGHT] = current_time

        # Move down
        if keys[pygame.K_DOWN] and current_time - self.last_move_time[pygame.K_DOWN] > self.move_delay:
            if self.valid_move(self.current_piece, self.current_piece['x'], self.current_piece['y'] + 1):
                self.current_piece['y'] += 1
                self.last_move_time[pygame.K_DOWN] = current_time

    def run(self):
        fall_time = 0
        fall_speed = 0.5  # Time in seconds before the piece falls one block
        
        while not self.game_over:
            fall_time += self.clock.get_rawtime()
            self.clock.tick()

            # Handle Pygame events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    return
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_UP:
                        # Rotate the piece if it's a valid move
                        rotated_piece = self.rotate_piece(self.current_piece)
                        if self.valid_move(rotated_piece, rotated_piece['x'], rotated_piece['y']):
                            self.current_piece = rotated_piece

            # Handle continuous movement (left, right, down)
            self.handle_continuous_movement()

            # Make the piece fall
            if fall_time / 1000 > fall_speed:
                if self.valid_move(self.current_piece, self.current_piece['x'], self.current_piece['y'] + 1):
                    self.current_piece['y'] += 1
                else:
                    # If the piece can't move down, place it and create a new piece
                    self.place_piece(self.current_piece)
                    rows_cleared = self.remove_full_rows()
                    self.score += rows_cleared * 100  # Increase score for cleared rows
                    self.current_piece = self.new_piece()
                    
                    # Check for game over
                    if not self.valid_move(self.current_piece, self.current_piece['x'], self.current_piece['y']):
                        self.game_over = True
                
                fall_time = 0  # Reset fall time

            # Draw the game state
            self.draw()

if __name__ == "__main__":
    game = Tetris()
    game.run()
    pygame.quit()