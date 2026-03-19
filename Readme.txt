Feladatot beküldő csapat adatai:
    Név: C--
    Iskolája: Irinyi János Református Oktatási Központ
    Felkészítő tanár neve: Borsodi Csaba
    Email cím: filkohaziflorian@irinyi-ref.hu
    Tagok nevei: Filkóházi Flórián, Bondár-Oláh Bence, Hegedűs István András

Programfejlesztői környezet leírása:
    Visual Studio Code:
        Verziója: 1.11.0
    Neovim:
        Verziója: 0.11.5
    Python:
        Verziója: 3.13.7
    cmake:
        Verziója: 4.2.3

Használt külső csomagok:
    Python:
        Dashboard:
            - customtkinter
            - CTkListbox
            - matplotlib

        Szimuláció:
            - pygame
    C++:
        - pybind11

Rövid használati útmutató:
    Dashboardot lehet külön is indítani, nem kell kötelezően a programból indítani.

    Első indítás előtt:
        Dashboard:
            Kötelezően telepíteni kell a fent felsorolt dolgokat (a külső csomagoknál a dashboard résznél). 
            Windowsban elég elíndítani az "install.bat" fájlt, ami letölti a szükséges Python csomagokat. Ehhez szükséges a telepített Python.
    
    Indítás:
        Dashboard:
            Lehet 2 féleképpen indítani:
                Parancssorban "python dashboard.py" vagy dupla kattintással:
                    Ilyenkor egyből a log választó nyílik meg ahol ki lehet választani melyik logot szeretnén megnézni.
                
                Parancssorban "python dashboard.py " és utána pedig beírva a log nevét (pl. "python dashboard.py "2026.03.19 18.53.0""):
                    Ilyenkor egyből már a főablak nyílik meg ahol már látható a kiválasztott log tartalma.
    
    Használti útmutató:
        Dashboard:
            Főoldal:
                A log betöltés folyamata (5 másodpercenként automatikusan frissül):
                    1. Ellenőrzi, hogy a log fájl mérete változott-e.
                    2. Ha igen, akkor ellenőrzi, hogy van-e új sor.
                    3. Ha van, akkor betölti az új sort.

                Kék "Újratöltés" gomb: Ez annyit csinál hogy manuálisan rá frissít a log betöltésre.

                A teljes nézet gombok: Ez annyit csinál hogy az adott diagramot nagyban jelenítik meg, minden adatot mutatva.

                    Teljes nézet:
                        Óra megjelenítése:
                            - 32 adat alatt minden időpont látszik.
                            - 32 adat felett csak a páros órák jelennek meg.
                
                A diagramok kis méretben 7 darab idő-ig ír ki adatokat.
                A "Gyűjtött Ásványok" diagramnál az érték zárójelben, százalék felül.
            
            Log választó:
                Kék "Újratöltés" gomb: Ez annyit csinál hogy frissíti a log mappát és a listát.
                Itt nincs automatikus időzítő.
                Ha nincs log akkor megjelelenik középen hogy "Sajnos nem található log.".
                Ha van log, akkor megjelenik a logok egy listában. Itt többet egyszerre nem lehet kijelölni.
                Egy log kijelölése után a továbbjutáshoz a kék "Kész" gombra kell nyomni.