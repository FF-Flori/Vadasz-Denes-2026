import pygame

class MainWindow:
    def __init__(self,width:int, height:int)->None:
        self.width = width
        self.height = height
        pygame.init()
        self.screen = pygame.display.set_mode((width,height))
        self.clock = pygame.time.Clock()
        self.running = True
    def mainloop(self)->None:
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

        self.screen.fill("black")

        #GAME idejön
        pygame.display.flip()

        self.clock.tick(60)
        return
    def quit(self)->None:
        pygame.quit()
