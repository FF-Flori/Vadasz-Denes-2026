import pygame


class HUD:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.img:pygame.Surface = pygame.image.load("./src/img/panel.png")
        self.img = pygame.transform.scale(self.img,windowsize)
    def show(self, screen:pygame.Surface)->None:
        # Aspect ratio is handled already so we dont need to check the height
        screen.blit(self.img,(0,0))
