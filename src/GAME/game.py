import pygame


class Game:
    def __init__(self) -> None:
        print("Initialized")
    def drawGame(self, screen:pygame.Surface) -> None:
        # Ide kerül majd minden rajzolás a game subclassaiból
        screen.fill("cyan")
