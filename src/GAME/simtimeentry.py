import pygame

class Question:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.texture:pygame.Surface = pygame.transform.scale(pygame.image.load("./src/img/menu.png"),windowsize)
        self.width:int = windowsize[0]
        self.height:int = windowsize[1]
        self.textcolor:tuple[int,int,int] = (255,207,161)

        self.buttonwidth:int = int(windowsize[0]*0.5) 
        self.buttonheight:int = int(self.buttonwidth/3)
        self.buttonPos:list[int] = [(windowsize[0]-self.buttonwidth)//2,int(windowsize[1]*0.7)]
        self.button:pygame.Surface = pygame.Surface((self.buttonwidth,self.buttonheight)).convert_alpha()
        self.normButton:tuple[float,float,float,float] = (self.buttonPos[0]/self.width,self.buttonPos[1]/self.height,self.buttonwidth/self.width,self.buttonheight/self.height)
        self.button.fill((0,0,0,0))
        pygame.draw.rect(self.button,(0, 0, 0),(0,0,self.buttonwidth,self.buttonheight),0,20)
        pygame.draw.rect(self.button,(255, 157, 111),(0,0,self.buttonwidth,self.buttonheight),3,20)

        buttontext:pygame.Surface = pygame.font.Font("src/font/PressStart2P-Regular.ttf",50).render("Tovább",True,self.textcolor)
        self.button.blit(buttontext,((self.buttonwidth-buttontext.get_width())//2,(self.buttonheight-buttontext.get_height())//2))
        self.lastClicked:bool = True

        self.text:pygame.font.Font = pygame.font.Font("src/font/PressStart2P-Regular.ttf",50)
        self.entryDims:list[int] = [0,int(windowsize[1]*0.2),3*windowsize[0]//4,100]
        self.entryDims[0] = (windowsize[0]-self.entryDims[2])//2
        self.entry:pygame.Surface = pygame.Surface((self.entryDims[2],self.entryDims[3])).convert_alpha()
        self.entry.fill((0,0,0,0))
        pygame.draw.rect(self.entry,(0, 0, 0),(0,0,self.entryDims[2],self.entryDims[3]),0,20)
        pygame.draw.rect(self.entry,(255, 157, 111),(0,0,self.entryDims[2],self.entryDims[3]),3,20)

        entrytext:pygame.Surface = pygame.font.Font("src/font/PressStart2P-Regular.ttf",40).render("Szimulációs idő: (x fél óra)",True,self.textcolor)
        self.texture.blit(entrytext, (self.entryDims[0]+10,self.entryDims[1]-entrytext.get_height()-10))

        self.simtime:int = 0
        self.enteredsimtime:str = ""
        self.reRender:bool = True #if true, rerender the text
        self.enteredText:pygame.Surface = pygame.Surface((0,0))
        self.textPos:tuple[int,int] = (0,0)
        self.keys = [pygame.K_0,pygame.K_1,pygame.K_2,pygame.K_3,pygame.K_4,pygame.K_5,pygame.K_6,pygame.K_7,pygame.K_8,pygame.K_9]
        self.keypressed:list[bool] = [False]*10
        self.backpressed:bool = False

    def update(self,windowDimensions:tuple[int,int,int,int])->bool:
        if pygame.mouse.get_pressed(3)[0]:
            if self.lastClicked:
                return False
            self.lastClicked = True
            pos = pygame.mouse.get_pos()
            pos = (pos[0]-windowDimensions[0],pos[1]-windowDimensions[1])
            normalized:tuple[float,float] = (pos[0]/windowDimensions[2],pos[1]/windowDimensions[3])
            if normalized[0] >= self.normButton[0] and \
               normalized[0] <= self.normButton[0]+self.normButton[2] and \
               normalized[1] >= self.normButton[1] and \
               normalized[1] <= self.normButton[1]+self.normButton[3]:
                    if len(self.enteredsimtime) > 0:
                       assert(self.enteredsimtime.isdecimal())
                       self.simtime = int(self.enteredsimtime)
                       if self.simtime > 0:
                           return True
                    else:
                        return False
        else:
            self.lastClicked = False

        keys = pygame.key.get_pressed()
        if len(self.enteredsimtime) < 21:
            for i in range(len(self.keys)):
                if keys[self.keys[i]]:
                    if self.keypressed[i]:
                        continue
                    self.keypressed[i] = False
                    self.enteredsimtime += str(i)
                    self.reRender = True
                    self.keypressed[i] = True
                else:
                    self.keypressed[i] = False
        else:
            for i in range(len(self.keys)):
                self.keypressed[i] = False
        if keys[pygame.K_BACKSPACE]:
            if self.backpressed:
                return False
            self.backpressed = True
            self.enteredsimtime = self.enteredsimtime[:-1]
            self.reRender = True
        else:
            self.backpressed = False

        return False
    def show(self,screen:pygame.Surface)->None:
        screen.blit(self.texture,(0,0))
        screen.blit(self.button,(self.buttonPos[0],self.buttonPos[1]))
        screen.blit(self.entry,(self.entryDims[0],self.entryDims[1]))

        if self.reRender:
            self.enteredText = self.text.render(self.enteredsimtime,True,self.textcolor)
            self.textPos = ((screen.get_width()-self.enteredText.get_width())//2,self.entryDims[1]+(self.entryDims[3]-self.enteredText.get_height())//2)
            self.reRender = False
        screen.blit(self.enteredText,self.textPos)
