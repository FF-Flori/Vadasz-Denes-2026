from src.GUI import MainWindow as game
import sys

errorstring:str = f"Helyes használat:\n./main.exe [időtartam]\n\tidőtartam: az az időtartam amit a szimuláció igénybe fog venni órában megadva. [24 - {2**15-1}]\n"
numin:int = -2
if len(sys.argv) > 1:
    param:str = sys.argv[1]
    if param == "--help" or param == "-h" or param == "--h" or param == "-help":
        print(f"\n./main.exe [időtartam]\n\tidőtartam: A szimuláció által kihasznált idő, órában megadva. Minimum: 24, Maximum:{2**15-1}.\n\tHa nem kap időtartam értéket a program, majd a grafikus felülettel kéri be a felhasználótól az időtartamot, ott már fél órákba.\n\tEzt az üzenetet a következő kapcsolókkal lehet elérni: `-h` `--h` `-help` `--help`.\n")
        exit()
    elif not param.isdecimal():
        print("\nHibás argumentum.",errorstring)
        exit()
    intform:int = int(param)
    if intform < 24 or intform > 2**15-1:
        print("\nA megadott érték a határokon kívülre esik.",errorstring)
        exit()
    numin = intform


window:game.MainWindow = game.MainWindow(700,900,"GAME")
window.game.gameHandler.inputtime = numin

window.mainloop()
window.quit()
