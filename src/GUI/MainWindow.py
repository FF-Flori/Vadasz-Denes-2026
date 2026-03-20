import sys
import os
try:
    import pygame

except ImportError:
    current_dir = os.path.dirname(os.path.abspath(__file__))
    libdeps_path = os.path.join(current_dir, '..', 'libdeps')

    if libdeps_path not in sys.path:
        sys.path.insert(0, libdeps_path)

    import pygame
from src.GUI.GameWindow import *

class MainWindow:
    def __init__(self,width:int, height:int, caption:str)->None:
        self.width = width
        self.height = height
        pygame.init()
        pygame.font.init()
        self.screen = pygame.display.set_mode((width,height),pygame.RESIZABLE)
        pygame.display.set_caption(caption)
        self.clock = pygame.time.Clock()
        self.running = True
        self.game:GameWindow = GameWindow(width,height)
        self.lastTime = pygame.time.get_ticks()
        self.deltaTime = 0.16666
    def mainloop(self)->None:
        while self.running:
            self.deltaTime = pygame.time.get_ticks()-self.lastTime
            self.lastTime = pygame.time.get_ticks()
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                if event.type == pygame.MOUSEWHEEL:
                    self.game.gameHandler.gameHandler.logicModule.zoom += event.y/10
                    self.game.gameHandler.gameHandler.logicModule.scale()

            self.screen.fill("black")

            self.game.showScreen(self.screen,self.deltaTime)
            pygame.display.flip()
            self.clock.tick(60)

        return
    def quit(self)->None:
        pygame.quit()
