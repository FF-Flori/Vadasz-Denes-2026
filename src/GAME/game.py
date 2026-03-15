import pygame
from src.GAME.gamepanel import *
from src.GAME.HUD import *

class Game:
    def __init__(self,windowsize:tuple[int,int]) -> None:
        self.gameHandler:GamePanel = GamePanel(windowsize)
        self.HUDHandler:HUD = HUD(windowsize)
    def drawGame(self, screen:pygame.Surface, deltaTime:float) -> None:
        self.gameHandler.updatePanel(deltaTime)
        self.HUDHandler.update(self.gameHandler.logicModule)
        self.HUDHandler.show(screen)
        self.gameHandler.showPanel(screen)

