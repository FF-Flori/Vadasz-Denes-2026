import pygame
from src.GAME.gamepanel import *
from src.GAME.HUD import *
from src.GAME.mainmenu import *
from src.GAME.simtimeentry import *

class Game:
    def __init__(self,windowsize:tuple[int,int]) -> None:
        self.inputtime:int = -2
        self.menu:int = 0
        self.gameHandler:GamePanel = GamePanel(windowsize)
        self.HUDHandler:HUD = HUD(windowsize)
        self.MenuHandler:MainMenu = MainMenu(windowsize)
        self.entryHandler:Question = Question(windowsize)
        self.initialized:bool = False
    def drawGame(self, screen:pygame.Surface, deltaTime:float,windowDimensions:tuple[int,int,int,int]) -> None:
        if self.menu == 0:
            if self.MenuHandler.update(windowDimensions):
                if self.inputtime > 0:
                    pygame.display.flip()
                    self.HUDHandler.setTime = self.inputtime*60
                    self.menu = 2
                    self.gameHandler.logicModule.SetTimeValue(self.inputtime*60)
                else:
                    self.menu = 1
            self.MenuHandler.show(screen)
        elif self.menu == 1:
            if self.entryHandler.update(windowDimensions):
                pygame.display.flip()
                self.menu += 1
                self.HUDHandler.setTime = self.entryHandler.simtime*30
                self.gameHandler.logicModule.SetTimeValue(self.entryHandler.simtime*30)
            self.entryHandler.show(screen)
        else:
            self.gameHandler.updatePanel(deltaTime)
            self.HUDHandler.update(self.gameHandler.logicModule)
            self.HUDHandler.show(screen)
            self.gameHandler.showPanel(screen)
