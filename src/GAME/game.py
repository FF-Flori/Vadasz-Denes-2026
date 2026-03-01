import pygame
from src.GAME.gamepanel import *

class Game:
    def __init__(self) -> None:
        self.gameHandler:GamePanel = GamePanel()
    def drawGame(self, screen:pygame.Surface, deltaTime:float) -> None:
        # Ide kerül majd minden rajzolás a game subclassaiból
        screen.fill("gray")
        self.gameHandler.updatePanel(deltaTime)
        self.gameHandler.showPanel(screen)

