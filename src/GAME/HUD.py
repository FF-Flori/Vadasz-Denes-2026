import pygame


class HUD:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.img:pygame.Surface = pygame.image.load("./src/img/panel.png")
        self.img = pygame.transform.scale(self.img,windowsize)
        self.margin:int = 15
        self.topmargin:int = windowsize[0]-2*7
        self.leftmargin:int = 25
        self.width:int = (windowsize[0]-2*self.margin) - self.leftmargin
        self.height:int = 190
        self.bluematerial:int = 0
        self.greenmaterial:int = 0
        self.yellowmaterial:int = 0
    def show(self, screen:pygame.Surface)->None:
        # Aspect ratio is handled already so we dont need to check the height
        screen.blit(self.img,(0,0))
        font = pygame.font.Font(None, 36)
        oresimg = pygame.image.load("./src/img/ores.png").convert_alpha()
        oresrect = pygame.Rect(0,0, 64, 64)
        oreswidth:int = 64
        fontcolor = (150, 150, 150)
        # Full screen: self.leftmargin, self.topmargin
        pygame.draw.rect(screen, (70, 70, 70), pygame.Rect(self.leftmargin, self.topmargin, self.width, self.height), border_radius=5)
        pygame.draw.rect(screen, (50, 50, 50), pygame.Rect(self.leftmargin+10, self.topmargin+5, 100, self.height-10), border_radius=5)
        
        #Kék anyag kirajzolása
        oresrect.x = oreswidth
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+9), oresrect)
        bluematerialtxt = font.render(str(self.bluematerial), True, fontcolor)
        screen.blit(bluematerialtxt, ((self.leftmargin+20)+65, (self.topmargin+10)+15))

        #Zöld anyag kirajzolása
        oresrect.x = int(oreswidth*3)
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+60), oresrect)
        greenmaterialtxt = font.render(str(self.greenmaterial), True, fontcolor)
        screen.blit(greenmaterialtxt, ((self.leftmargin+20)+65, (self.topmargin+60)+15))

        #Sárga anyag kirajzolása
        oresrect.x = int(oreswidth*2)
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+120), oresrect)
        yellowmaterialtxt = font.render(str(self.yellowmaterial), True, fontcolor)
        screen.blit(yellowmaterialtxt, ((self.leftmargin+20)+65, (self.topmargin+120)+15))

