#include "definitions.h"

#include "sample_facelets.h"

char sample_facelets[N_FACELETS_SAMPLES][N_FACELETS] = {
    "BBFRURDDLBFRDRFDLDFLUDFRFDFLBLUDBRULDURULFBLUURRLBFBBU", "BBURUDBFUFFFRRFUUFLULUFUDLRRDBBDBDBLUDDFLLRRBRLLLBRDDF",
    "BBURULRUULFLBRRBFBDFBFFDFRDUULUDRDDUDBFLLDFBRFLRDBURLL", "BDBBULRBRFBLLRDFFBURUDFUDUULFLUDRRFDDUBLLRFRFUBRLBFLDD",
    "BDBDURFFRDFDRRLUBLLLFBFURFBBULDDURLDRFURLRUBDLLUUBDFBF", "BDFBUUFDLUFDLRUFURDBFLFDUURLBUDDRDLBDRLFLFLRBRFRLBRUBB",
    "BDLLURUFRDBDRRRDFURLFLFUBFLRDBLDUBBLDDFFLBLUUFBRDBRFUU", "BDLRULLRRDFFBRLDRLBFFFFUUDBBRRLDURDDLBULLUUBRUBDUBDFFF",
    "BDRLUBDBDFRBFRRBFFRURRFUDDDLLLUDLFDUUUBBLFRRFUBLDBLLFU", "BFDDURFBRDBLURLBUUDLFRFBDRURURLDLFFFUBLFLDUFBBRLDBDRUL",
    "BFFBUFUBFDRDDRDFRDBRLFFBBDUDLLBDUUFLUURLLLFLRRDLRBUBUR", "BFRFUBBLBLRFDRBDUUUBDFFRLFFULLDDRFDRDULLLRRLFDDRUBUBBU",
    "BFRLURUFFLFBBRRDLDLLUDFRULFRDLFDURBRLBBRLBDUFUDDDBUFUB", "BLBDUULUDBLRDRFLBUUBLRFFURDLDFRDRRUFRBFLLUFFBDDULBBRFD",
    "BRBFUURBFRRDURLBDDFRDFFBBLRUDDLDRUBFRUUFLDLULLFUBBLLDF", "BRDBUFBFBUURBRRFUUDLLDFDDDRLFULDLBULRURLLRDBFFFUBBDFRL",
    "BRDFUBUBULULRRUDFUFRBDFUDFFRDLDDLFLFRULLLRDBBBFULBBRDR", "BRDFUUUUULBFDRDDDFRLBUFRLBBUDRFDLBLDRRFBLFDLFLUUFBRRBL",
    "BRLFUDLDRFLUFRUFFLFBURFRUUDFBRUDDRRBRLDLLBDLLBUUFBBDDB", "BRRLUFBFFLRFDRUDLURDUFFRULFRDLRDUDBBLFULLUBUFDBDBBBLDR",
    "BULRUULFRDBDDRLUDRULFFFBLUBDRLFDLURBDBFLLDFRBFFRUBBUDR", "BUULURRDFRURFRBUFUBLDDFLFBFDRRBDRUULLBDRLFBULBFDDBDFLL",
    "DBRDULBFBUDUDRRULLDRLLFRLBRFUBUDUDDFBFRBLFRFDFLLUBRUBF", "DBRLUFFUBDLFBRFLBLLFRFFUBLBRUDDDDFUULDUBLRRRUDLFDBRBRU",
    "DBUFUFDFUBLLLRURUBLRRDFBFLDUUBFDRDDLFUFBLLLDRFDRBBRURB", "DDRBUUBUFUBFURLBRFDFRRFRFFULLRDDBLULBDRLLFUFUDRLDBBDLB",
    "DFRBURUDDLBDRRFUDULLFLFUULFFBRRDRFBRBDBFLUDFLBULDBLBUR", "DRDUUBRRLBLLRRLUFDUFULFBFDLULFDDUDBBFRBDLUFFRBDRFBBRUL",
    "DRLUUBFBRBLURRLRUBLRDDFDLFUFUFFDBRDUBRUFLLFDDBFLUBLRBD", "DRRUURBRURUBDRLUBFDBFBFBLFLDRFFDULLDLFRDLLUDBUDFUBLRFB",
    "DRUUUDDBRULLBRBFFLLLBFFDBURLFUUDLRRURRFFLDFLDFDBUBRBBD", "DUBBUFLFFLRLDRBBLRDUUDFLLRRFBDDDURRUFLBFLRBFDUBRDBLFUU",
    "DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL", "DUFBUDBRDBRUFRURRBDFLLFDDLBLFULDURBLFDRDLBUUFLBRFBLURF",
    "DURBULRULBUDRRRLDLDBULFFFBBULDDDRBBFLDFDLFUFRBFFUBLURR", "DURFULUBUFDFLRFRUBLRLRFBFDBUBUFDRDFRBRBULDFLRDLLDBBDUL",
    "FBBDULLFRUBRLRLDFUDRBBFFUURLLFUDDDBLURBFLRFRBDURDBUFDL", "FBRDULFBRFBBRRRDLBRRUDFUUDFLBLFDUBFDLLDLLFRRBUUUDBFLUD",
    "FBUDUBBRBUURLRRUFLDBLLFDRFFDLRFDRFUBDBRRLUUDFBLLUBDDFL", "FDBBUDUBRDBLLRFBRRBUFRFULDDUFLFDURLUDRRLLDDUFULLRBFFBB",
    "FDLBUBRLUBUFDRURRBDURFFLLDUDFFRDFDLLDLFRLUBBBUBLRBDUFR", "FDLUUURLFLBDFRRDLBBBUBFLDDLFBBUDUBRLURDFLRUFRFLRFBDUDR",
    "FDRUUBBBLDRBDRLFFFDUBLFRBURURUFDRFFUDLRFLBDLLUBLDBDLUR", "FFRRULFBDRFBURLFBUDLBRFFDBRBUUDDDLRLRULRLDULLUDDUBFFBB",
    "FLBUUDFBDBFURRRUBLLULRFFLLFBDRBDLDRDRLUFLDBDURFDBBUFUR", "FLBUULFFLFDURRDBUBUUDDFFBRDDBLRDRFLLRLRULFUDRRBDBBBUFL",
    "FLDRUBBRUBDRBRRLRFDFRDFUUUBFFDDDUULRDDRLLFBLLFULBBFUBL", "FLLUUULLULBDRRFUFDUDBBFBBFRLRBBDURDBURFDLLFDDFURLBRRFD",
    "FLURURDBRBDBBRUUUUFLDFFULUFFLRDDRLBRLFRFLLBBDLDUFBDBRD", "FRBUURUDBLBRDRLDFDRBDDFLUFFLULDDLBLRLBFFLRUFBDUUUBRFBR",
    "FRRBUBDDURLBDRDLUDRLFLFBLBFDUUFDRLUFRRBRLFUUBUDDFBFLLB", "FRRUUFURDBLFRRFFLUBDLRFBLFUFULLDDBBBULRDLURBDDFRDBBLUD",
    "FUBUUFLBLURRDRFFUDDUFBFRUBDBLRRDLBFFURBLLDLBRDFRLBDLDU", "FUDLUBDLFLRBURFUDLLDUFFLBUFURRBDFRDBRFFDLRBLLRBDUBRDBU",
    "FUFBUBURDFLRDRFBUFLDRFFLBDDDBLFDLURLLUBFLURDRURULBRDBB", "FURBUDLRRUBDFRLBDFBUBFFRUDDRRLDDFULLRUUBLULLFBLDBBRDFF",
    "FURFUDBUDFLBFRBDRDUBRBFRULBFFRBDULDFUULRLRBDLULRLBDLFD", "LBLLUBUFUBRUURFDLRRLRDFLDFLFRBRDDLBDFBFRLFFDRBUDUBUBDU",
    "LDRLUDRLFURBRRRRDDBBRFFUUBFLUDUDLLBFUFDFLUBLBUBFFBDLRD", "LDUFULFRURDRLRULBDDFFRFBFBURDBBDUBFLULLDLURRDBFFLBRBUD",
    "LFDLUUDRDBBBRRUBFLFFLBFURDUFBRUDDUBFFDRRLLBLURLDFBDURL", "LFFDUBLRDLLDURLBDFDUFLFFFDUUBRRDRDBUBLBRLUBBRRDUFBFLUR",
    "LFRLUUBBLBFUDRRUUFDDUDFFULLBUFLDBLFDFDRBLRBBRFRDUBRRLD", "LFRUULBBDBFBDRBLFRDDLLFLFRDRBFDDUBUUURRFLBLRDURFUBDFLU",
    "LLBDURRDURBDLRLRULBBFBFDDBBFUUFDRLFFBRDFLLDDRLUUFBRUUF", "LLBRUFUBBUURURDLLRBDLRFBBLFLUUDDFRRFFDRRLFUFDDBDLBBDUF",
    "LLFLUBRURDLUFRDRURUBFRFULBDDRBRDRLDUBUBDLDDFBLFUBBFFLF", "LLRDUBUBBURBBRBLRRRLLLFUBFDDRFFDUULFDFFDLULURUDBDBRDFF",
    "LLURULDUDLBLURBBRFRFFRFLFRURBRFDDBBRDFBFLULDDFDBUBLUDU", "LRBUULRDRDBLLRBDDUFRFDFURLLBFBUDBBFLFBUDLFURDUFDRBLFUR",
    "LRLLURRLBDUULRFUFLFDRUFBRBLDUFDDRRBDDFURLLBFFBDBUBBFDU", "LRRFUUBDLFBBLRDLLRUFDLFURLFBBUBDDDDUDULRLFRRDUUBRBFFBF",
    "LUFBUUUDDLLRDRRDDLBFFFFRUDBLLRRDBRBFDURFLRFUBUFBBBLULD", "LUFFUURDFDLURRDBBFUBLUFFBRDDDLLDRRLDBLBBLRUDRLFUFBURBF",
    "RBBDUBBRRFLDFRFFRRDUUFFUDFULLLLDBBLBDBRDLRLUFLUFDBRUDU", "RFLUUFFDFDRBLRBRBRULLBFFUUFFFDLDDUDDURRBLRBULDDBLBUBRL",
    "RLFDUFFBLDUDBRLDFFLDBUFULLBBDRRDDFURURURLLLBURFBBBFURD", "RLRUURDLFUFDBRLFRBLFRFFDDLUFULRDUDFLUBFRLULBRBDBBBDUDB",
    "RRRUURUUBDDBFRBFLURLRFFRLULDBDLDBLLLDFFULDUDBUBFDBRFFB", "RURFUDDDFLBBFRULUBFFUDFLDLDRBFBDLBDLFRRULRDRBURUBBFULL",
    "UBBRUDBLFRBRRRUFBBLFDDFFFULUFDDDLBLUFUDBLRLFRDULLBRRDU", "UBLLURBDLDDFRRLRFLDFBLFURFFFLDBDULBUBDRRLBDDUUURUBFBRF",
    "UDDBURDDBUFLFRLFDFRRLRFLUBRFDUBDFRFLBLBRLUFULBLRUBBDUD", "UDLFUBLRFRDDLRFDBDBBDDFUUUBFRRLDUFFLBLUFLRRBLFLRUBDBRU",
    "UDRFULDRBLDUBRDUFBFBDRFDFURLBBLDRDURLLRULUFBUFRBFBFDLL", "UFBBURLLBRFDBRRFURBDUDFLDBDLDRFDLUBDRUUFLRLDFLLFUBUBRF",
    "UFBLURRDLDURRRLUFFFBBFFDULRFFBRDUFBDLBUDLRLBLDDBUBLRUD", "UFFDUULUUBFRLRFLDUDLRRFBBRDDUFDDFFBLLLBRLDDBRURBLBBFUR",
    "UFLRULBLUBUBBRFRDFDFRBFLFUFLRDDDRBBLRFRBLULLUDDFUBDDRU", "ULBFURBDBDDDFRRBFFUBRLFUFRURBRUDDDLURRLLLFFBDLDFUBULBL",
    "ULDFUBBRRBDLFRBFFLUUDBFLRFDUDRLDUFUDRRLDLUUDBBBFRBRFLL", "ULFUUURRURLLLRRUBLDFFLFFBDRURBRDDDFBFBFFLDBBLDBLUBDDUR",
    "ULLLURFBBDUBBRUBLFURRDFDRFUDRRFDBUULLURRLLLDFDFBBBDDFF", "URBLUBDDBURDLRDLDBFBLLFFULDBBFUDFLRRRURBLDFFRLUFRBUDFU",
    "URBLUBFDUBRRLRFLULRLRBFFLDDBFFLDRFFBRBDBLUUUUDDFRBDDUL", "URDBUDLLBDFLBRUDLFBDRBFDDURLLFRDBULRBUUDLRLUFBFRFBRUFF",
    "URLBULDDULDBBRLRRFRRBFFLFLFDFDUDUURRBUBBLDLFLDFRUBDUBF", "UUBUUBUDBDRLFRFDUDBBRBFRFBRUUFDDLLDFFRRDLLBRRUFLLBFLLD",
    "UULFUFDLRFDBBRDBLFLFULFUDDURBRRDDURDFUFLLBBFBDRLRBURBL", "UUULULFLFDUBDRRDBBUFLFFLBRRLDFFDDBUUFBRRLDRUDLBLFBBRRD"};