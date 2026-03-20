> Ez a 2026-os Vadász Dénes informatika verseny, C-- csapatának útvonaltervező algoritmus bemutatója.

# Videó script
Projektünkben az útvonal generálását egy saját fejlesztésű C++ modul végzi, amely a Pathfinder nevet kapta.
A Pathfinder-t a Pybind11 segítségével Pythonból tudjuk kezelni.

Az útvonaltervezést 6 fázisra bonthatjuk:
- Inicializálás
- Érc-csoportosítás
- Útvonaltervezés
- Útonaloptimalizálás
- Végleges útvonal kiszámolása
- Visszatérés


## Inicializálás
A Pathfinder példányosításakor szükség van az időkorlátra és a pálya .csv fájljának abszolút elérési útvonalára.
A konstruktor beolvassa a pályát és előkészíti a többi lépéshez szükséges erőforrásokat.

## Érc-csoportosítás
Ahhoz, hogy a program futási idejét vállalható intervallumba tereljük, fel kell áldoznunk egy keveset a lehetőségek számából.
Ezt úgy tesszük meg, hogy az egymás mellett elhelyezkedő érceket egyesítve érc-csoportokat hozunk létre.
A csoportok méretét a GROUP_LIMIT konstans szabályozza.
Minden ehhez hasonló, fordítás előtt konfigurálható konstans érték a pathfinder.hpp elején található.

## Útvonaltervezés
Az így létrejött érc-csoportok közötti összeköttetést egy A* algoritmussal teremtettük meg.
A csoportok közé bevettük a kezdőpontot is és így egy teljes gráfot hoztunk létre, amelynek élei az állomások közötti útvonalak lettek.

## Útvonaloptimalizálás
Az ideális útvonal kiválasztását egy genetikus algoritmus végzi.
Ez nagyvonalakban azt jelenti, hogy először generálunk sok véletlen sorozatot, ami az állomások meglátogatási sorrendjét adja, majd a jó sorozatokat visszük tovább, valamit mutációkat hajtunk végre rajtuk.
Így egy jópár iteráció után kapunk egy használható útvonalat.

A genetikus algoritmus fontos része az ún. 'fitness' függvény, ami egy adott sorozatról állapítja meg, hogy az mennyire jó.
A 'fitness' függvény egy pontszámot ad a sorozatoknak, ami alapján eldöntjük, hogy a következő iterációra melyiket visszük tovább.
Nálunk a pontszám a sikeresen kiszedett ércek száma az időkorláton belül.

## Végleges útvonal kiszámolása
A genetikus algoritmus által visszaadott legjobb sorozaton még optimalizálni kell a sebességfokozatokat, ezért egy újabb algoritmust vetettünk be, ami a BFS (Breadth First Search) rövidítést kapta.
Ez az algoritmus elkezdi keresni a használható sebességfokozatokat az útvonalon, közben egy táblát tölt fel, amiben pozíció, következő állomás és eltelt idő alapján különíti el az állapotokat.
A BFS algoritmus alapelve, hogy a leghamarabb célt érő állapotsorozat lesz a legoptimálisabb.
Az érintett állapotok visszakövetésével megkapjuk a teljes útvonalat a használt sebességfokozatokkal együtt.

## Visszatérés
A kapott instrukciók sorozatát visszaadjuk egy instructions_t nevű struktúrában, ami a szükséges műveleteket négyes bitcsoportokban tárolja.
Az így eltárolható maximum 16 féle utasításba könnyedén beleférnek a következők:
- 8 irányutasítás (fel, bal-fel, jobb-fel ...stb.)
- 4 sebességváltó utasítás (set_speed_3, set_speed_0 ...stb.)
- A bányászás

Az insrtukciók között helyet kapott még egy no_instruction nevű utasítás, amit az instructions_t padding-ként használ és a szimulációban nem kerül időbe vagy energiába.