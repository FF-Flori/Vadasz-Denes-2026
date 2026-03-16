import pygame

class MainMenu:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.texture:pygame.Surface = pygame.Surface(windowsize)
        self.texture.fill((100,125,255))
    def show(self,screen:pygame.Surface)->None:
        screen.blit(self.texture,(0,0))
