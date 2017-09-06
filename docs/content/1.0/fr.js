if (content["1.0"] === undefined)
	content["1.0"] = {};
content["1.0"]["fr"] = {};

content["1.0"]["fr"]["ajouter-des-variables-au-script"] = "" + 
	"<h1 id=\"ajouter-des-variables-au-script\">Ajouter des variables au script</h1>\n" +
	"<p>Dans cette section, nous allons voir comment créer une fonction &quot;<strong>print</strong>&quot; qui affichera les variables transmises et nous l'ajouterons au script avant de l'exécuter.</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// On déclare une fonction qui :\n" +
	"// - prend un état de script et une liste d'arguments en paramètres\n" +
	"// - retourne une variable en résultat\n" +
	"Variable CallPrint(State&amp; state, std::vector&lt;Variable&gt;&amp; args) {\n" +
	"    std::cout &lt;&lt; \"&gt; \";\n" +
	"    for (unsigned int i = 0; i &lt; args.size(); i++) {\n" +
	"        std::cout &lt;&lt; args[i];\n" +
	"        if (i != args.size() - 1)\n" +
	"            std::cout &lt;&lt; \", \";\n" +
	"    }\n" +
	"    std::cout &lt;&lt; std::endl;\n" +
	"    return CreateUndefined(state.memory);\n" +
	"}\n" +
	"\n" +
	"// Code source du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state(memory);\n" +
	"    // Création et déclaration de la variable\n" +
	"    DeclareVariable(state, \"print\", CreateVariable(memory, CallPrint));\n" +
	"    // Exécution du script\n" +
	"    state.Execute(code);\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p>Ainsi dans le script, l'appel à la fonction pourra se faire comme suit :</p>\n" +
	"<pre><code class=\"litescript\">print(\"une-chaine\", 24, true);</code></pre>";

content["1.0"]["fr"]["ajouter-lapi-dans-un-projet"] = "" + 
	"<h1 id=\"ajouter-lapi-dans-un-projet\">Ajouter l'API dans un projet</h1>\n" +
	"<p>Cette page détaille comment ajouter l'API dans un projet C++ pour l'utiliser.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"télécharger-les-sources\">Télécharger les sources</h2>\n" +
	"<p>Tout d'abord vous devez téléchargez les sources soit avec le <a href=\"https://github.com/LePtitDev/lite-script/archive/master.zip\">lien direct</a>, soit sur la <a href=\"https://github.com/LePtitDev/lite-script\">page Github</a> du projet.</p>\n" +
	"<h2 id=\"compiler-les-sources-dans-le-projet\">Compiler les sources dans le projet</h2>\n" +
	"<p>Ensuite, déplacez tous les fichiers se trouvant dans le dossier &quot;<strong>src</strong>&quot; de l'API téléchargée dans le répertoire de votre projet à l'emplacement souhaité.</p>\n" +
	"<p>Exemple :</p>\n" +
	"<pre><code>&lt;votre-projet&gt;/\n" +
	"  main.cpp\n" +
	"  include/\n" +
	"    litescript/\n" +
	"      &lt;fichiers-sources&gt;\n" +
	"      ...</code></pre>\n" +
	"<p>Enfin il vous suffit de compiler tous les fichiers &quot;<strong>.cpp</strong>&quot; de l'API en plus de vos fichiers sources à vous.</p>\n" +
	"<p>Exemple commande UNIX :</p>\n" +
	"<pre><code class=\"bash\">g++ main.cpp include/litescript/*/*.cpp -o main</code></pre>\n" +
	"<h2 id=\"compiler-avec-cmake\">Compiler avec CMake</h2>\n" +
	"<p>Un fichier CMakeLists.txt est déjà présent dans le dossier &quot;<strong>src</strong>&quot; permettant de faciliter la compilation avec CMake. On part du projet suivant :</p>\n" +
	"<pre><code>main.cpp\n" +
	"CMakeLists.txt\n" +
	"litescript/\n" +
	"  CMakeLists.txt\n" +
	"  &lt;fichiers-sources&gt;\n" +
	"  ...</code></pre>\n" +
	"<p>Le fichier CMakeLists.txt dans le répertoire &quot;litescript&quot; est déjà présent et contient le code suivant :</p>\n" +
	"<pre><code class=\"cmake\"># Define source files\n" +
	"set(SOURCE_FILES\n" +
	"        litescript.hpp\n" +
	"        nullable.hpp\n" +
	"        streamer.hpp\n" +
	"\n" +
	"        memory/type.hpp memory/type.cpp\n" +
	"        memory/object.hpp memory/object.cpp\n" +
	"\n" +
	"        types/internal.hpp types/internal.cpp\n" +
	"        types/undefined.hpp types/undefined.cpp\n" +
	"        types/null.hpp types/null.cpp\n" +
	"        types/boolean.hpp types/boolean.cpp\n" +
	"        types/number.hpp types/number.cpp\n" +
	"        types/string.hpp types/string.cpp\n" +
	"        types/character.hpp types/character.cpp\n" +
	"        types/callback.hpp types/callback.cpp\n" +
	"        types/array.hpp types/array.cpp\n" +
	"        types/class.hpp types/class.cpp\n" +
	"        types/class_object.hpp types/class_object.cpp\n" +
	"        types/namespace.hpp types/namespace.cpp\n" +
	"\n" +
	"        memory/memory.hpp memory/memory.cpp\n" +
	"        memory/variable.hpp memory/variable.cpp\n" +
	"        memory/basic_memory.hpp memory/basic_memory.cpp\n" +
	"\n" +
	"        script/instruction.hpp script/instruction.cpp\n" +
	"        script/namer.hpp script/namer.cpp\n" +
	"        script/state.hpp script/state.cpp\n" +
	"        script/executor.hpp script/executor.cpp\n" +
	"        script/assembly.hpp script/assembly.cpp\n" +
	"        script/syntax.hpp script/syntax.cpp\n" +
	"        script/script.hpp script/script.cpp\n" +
	"\n" +
	"        api/types.hpp api/types.cpp\n" +
	"        api/script.hpp api/script.cpp)\n" +
	"\n" +
	"# Add the library\n" +
	"add_library(LiteScript ${SOURCE_FILES})\n" +
	"\n" +
	"# Define target\n" +
	"target_include_directories (LiteScript PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})</code></pre>\n" +
	"<p>Vous devrez créer un autre fichier CMakeLists.txt mais cette fois-ci à la racine du projet comme montré précédemment et y écrire le code suivant :</p>\n" +
	"<pre><code class=\"cmake\"># Indiquez la version de votre projet\n" +
	"cmake_minimum_required(VERSION 2.8.4)\n" +
	"# Indiquez le nom de votre projet\n" +
	"project(MonProjet)\n" +
	"\n" +
	"# Initialisez les flags du compilateur\n" +
	"set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -std=c++11\")\n" +
	"\n" +
	"# Ajoutez le répertoire contenant les fichiers sources de l'API\n" +
	"add_subdirectory (litescript)\n" +
	"\n" +
	"# Créez l'exécutable\n" +
	"add_executable (executable main.cpp)\n" +
	"\n" +
	"# Ajoutez l'API à l'exécutable\n" +
	"target_link_libraries (executable LINK_PUBLIC LiteScript)</code></pre>";

content["1.0"]["fr"]["créer-un-type-de-variable"] = "" + 
	"<h1 id=\"créer-un-type-de-variable\">Créer un type de variable</h1>\n" +
	"<p>A l'aide des fichiers d'en-têtes inclus par le fichier <code>litescript.hpp</code> il est possible de créer des types supplémentaires de variables au même titre que les types <em>UNDEFINED</em>, <em>NIL</em>, <em>BOOLEAN</em>, etc... Vous pouvez voir la liste des types dans la catégorie <a href=\"#litescript__type\">LiteScript::Type</a>.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"un-nouveau-type-de-base\">Un nouveau type de base</h2>\n" +
	"<p>Pour ce faire, il va falloir dans un premier temps créer une classe qui hérite de <a href=\"#litescript__object\">Type</a>.</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"// L'intégralité des fonctions et classes de l'API se trouvent\n" +
	"// dans l'espace de noms LiteScript.\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"class MonType : public Type {\n" +
	"\n" +
	"public:\n" +
	"\n" +
	"    // La classe Type ne contient pas de constructeur par défaut.\n" +
	"    // Le constructeur de base prend en paramètre le nom du type.\n" +
	"    MonType() : Type(\"MonType\") {}\n" +
	"\n" +
	"    // Cette méthode doit obligatoirement être redéfinie car elle\n" +
	"    // est abstraite, son rôle est d'assigner les valeurs par\n" +
	"    // défaut dans l'objet.\n" +
	"    Object&amp; AssignObject(Object&amp; object) override {\n" +
	"        // Ici on assigne dans l'objet son type (là c'est MonType)\n" +
	"        // puis on indique le nombre d'octets nécessaire aux\n" +
	"        // données. Pour le moment nous n'en mettrons pas.\n" +
	"        object.Reassign(*this, 0);\n" +
	"    }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// Enfin on crée le type dans une variable globale, vous pouvez\n" +
	"// aussi le créer dans un attribut statique\n" +
	"MonType montype;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p>Ce code est le minimum syndical pour créer un type. Maintenant il va falloir que vous regardiez dans le détail comment fonctionne la classe <a href=\"#litescript__object\">Object</a> et les méthodes virtuelles de la classe <a href=\"#litescript__type\">Type</a> pour comprendre la suite.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"création-dun-booléen-à-trois-états\">Création d'un booléen à trois états</h2>\n" +
	"<p>Maintenant que vous connaissez la base, on va expérimenter ça sur un exemple simple, le type &quot;<em>booléen à trois états</em>&quot;.</p>\n" +
	"<blockquote>\n" +
	"<p><em>Quoi ? Un booléen à trois états ?</em></p>\n" +
	"</blockquote>\n" +
	"<p>Oui, pour faire simple, quand un booléen classique a deux états (&quot;<em>vrai</em>&quot; ou &quot;<em>faux</em>&quot;), un booléen à trois états peut avoir les états &quot;<em>vrai</em>&quot;, &quot;faux&quot; ou &quot;<em>inconnu</em>&quot;, on appelle ça la logique ternaire (plus d'infos <a href=\"https://fr.wikipedia.org/wiki/Logique_ternaire\">ici</a>). Bref, on va créer un type booléen à trois états qui aura les opérateurs de base d'assignation, d'égalité et d'inégalité et les opérateurs logiques à savoir la négation (<strong>not</strong> en anglais), l'intersection (<strong>and</strong>) et l'union (<strong>or</strong>).</p>\n" +
	"<p>Voici la table de vérité de la logique ternaire :</p>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>A</th>\n" +
	"<th>B</th>\n" +
	"<th>non A</th>\n" +
	"<th>A et B</th>\n" +
	"<th>A ou B</th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td>Faux</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Faux</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Faux</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Vrai</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Faux</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Inconnu</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Vrai</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Vrai</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Vrai</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Vrai</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Vrai</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Vrai</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Faux</td>\n" +
	"<td>Inconnu</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Vrai</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Vrai</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Inconnu</td>\n" +
	"<td>Inconnu</td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<blockquote>\n" +
	"</blockquote>\n" +
	"<p>Et maintenant le code source du nouveau type :</p>\n" +
	"<pre><code class=\"cpp\">// Nécessaire pour l'utilisation des std::allocator\n" +
	"#include &lt;memory&gt;\n" +
	"\n" +
	"#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// On déclare une classe qui servira de donnée à insérer\n" +
	"// dans l'Object\n" +
	"class Trilean {\n" +
	"\n" +
	"    // Valeur ternaire\n" +
	"    // 1: vrai\n" +
	"    // 0: inconnu\n" +
	"    // -1: faux\n" +
	"    char value;\n" +
	"\n" +
	"public:\n" +
	"\n" +
	"    // Le constructeur par défaut assigne la valeur \"inconnu\"\n" +
	"    Trilean() : value(0) {}\n" +
	"\n" +
	"    // Retourne la valeur\n" +
	"    char Get() const { return value; }\n" +
	"\n" +
	"    // Assigne la valeur\n" +
	"    void Set(char v) { value = v; }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// On déclare notre nouveau type\n" +
	"class TypeTrilean : public Type {\n" +
	"\n" +
	"public:\n" +
	"\n" +
	"    // On lui donne le nom \"Trilean\"\n" +
	"    TypeTrilean() : \"Trilean\" {}\n" +
	"\n" +
	"    // On surcharge la méthode abstraite\n" +
	"    Object&amp; AssignObject(Object&amp; object) override {\n" +
	"        // On alloue l'espace nécessaire à la classe Trilean\n" +
	"        object.Reassign(*this, sizeof(Trilean));\n" +
	"        // On utilise un allocateur pour initialiser l'élément\n" +
	"        // de la classe Trilean\n" +
	"        std::allocator&lt;Trilean&gt; allocator;\n" +
	"        allocator.construct(&amp;object.GetData&lt;Trilean&gt;());\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'assignation\n" +
	"    Variable OAssign(Variable&amp; obj1, const Variable&amp; obj2) const override {\n" +
	"        if (obj2-&gt;GetType() != *this) {\n" +
	"            // Si le type est différent, on change le type de \"obj1\"\n" +
	"            obj2-&gt;GetType().AssignObject(*obj1);\n" +
	"            obj1-&gt;GetType().OAssign(obj1, obj2);\n" +
	"        }\n" +
	"        else {\n" +
	"            // Sinon on assigne la valeur\n" +
	"            obj1-&gt;GetData&lt;Trilean&gt;().Set(obj2-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"        }\n" +
	"        // Puis on retourne le résultat\n" +
	"        return Variable(obj1);\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'opération d'égalité\n" +
	"    Variable OEqual(const Variable&amp; obj1, const Variable&amp; obj2) const override {\n" +
	"        Variable res = obj1-&gt;memory.Create(Type::BOOLEAN);\n" +
	"        if (obj2-&gt;GetType() != *this)\n" +
	"            res-&gt;GetData&lt;boolean&gt;() = false;\n" +
	"        else\n" +
	"            res-&gt;GetData&lt;boolean&gt;() = (obj1-&gt;GetData&lt;Trilean&gt;().Get() == obj2-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'opération d'inégalité\n" +
	"    Variable ONotEqual(const Variable&amp; obj1, const Variable&amp; obj2) const override {\n" +
	"        Variable res = obj1-&gt;memory.Create(Type::BOOLEAN);\n" +
	"        if (obj2-&gt;GetType() != *this)\n" +
	"            res-&gt;GetData&lt;boolean&gt;() = true;\n" +
	"        else\n" +
	"            res-&gt;GetData&lt;boolean&gt;() = (obj1-&gt;GetData&lt;Trilean&gt;().Get() != obj2-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'opération de négation\n" +
	"    Variable OLogicalNot(const Variable&amp; object) const override {\n" +
	"        Variable res = object-&gt;memory.Create(*this);\n" +
	"        res-&gt;GetData&lt;Trilean&gt;().Set(-1 * object-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'opération de négation\n" +
	"    Variable OLogicalAnd(const Variable&amp; obj1, const Variable&amp; obj2) const override {\n" +
	"        Variable res = obj1-&gt;memory.Create(*this);\n" +
	"        switch (obj1-&gt;GetData&lt;Trilean&gt;().Get()) {\n" +
	"        case -1:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(-1);\n" +
	"            break;\n" +
	"        case 0:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(obj2-&gt;GetData&lt;Trilean&gt;().Get() == -1 ? -1 : 0);\n" +
	"            break;\n" +
	"        case 1:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(obj2-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"            break;\n" +
	"        }\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge l'opération de négation\n" +
	"    Variable OLogicalOr(const Variable&amp; obj1, const Variable&amp; obj2) const override {\n" +
	"        Variable res = obj1-&gt;memory.Create(*this);\n" +
	"        switch (obj1-&gt;GetData&lt;Trilean&gt;().Get()) {\n" +
	"        case -1:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(obj2-&gt;GetData&lt;Trilean&gt;().Get());\n" +
	"            break;\n" +
	"        case 0:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(obj2-&gt;GetData&lt;Trilean&gt;().Get() == 1 ? 1 : 0);\n" +
	"            break;\n" +
	"        case 1:\n" +
	"            res-&gt;GetData&lt;Trilean&gt;().Set(1);\n" +
	"            break;\n" +
	"        }\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"    // On surcharge la méthode ToString pour pouvoir afficher\n" +
	"    // plus facilement le contenu de la variable\n" +
	"    std::string ToString(const Variable&amp; object) const override {\n" +
	"        std::string res;\n" +
	"        switch (object-&gt;GetData&lt;Trilean&gt;().Get()) {\n" +
	"        case -1:\n" +
	"            res = \"faux\";\n" +
	"            break;\n" +
	"        case 0:\n" +
	"            res = \"inconnu\";\n" +
	"            break;\n" +
	"        case 1:\n" +
	"            res = \"vrai\";\n" +
	"            break;\n" +
	"        }\n" +
	"        return res;\n" +
	"    }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// On crée la variable globale\n" +
	"TypeTrilean typeTrilean;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // On crée une mémoire\n" +
	"    Memory memory;\n" +
	"    // On crée trois variables de type \"Trilean\" ...\n" +
	"    Variable    v1 = memory.Create(typeTrilean),\n" +
	"                v2 = memory.Create(typeTrilean),\n" +
	"                v3 = memory.Create(typeTrilean);\n" +
	"    // ... que l'on assigne différemment\n" +
	"    v1-&gt;GetData&lt;Trilean&gt;().Set(-1);\n" +
	"    v2-&gt;GetData&lt;Trilean&gt;().Set(0);\n" +
	"    v3-&gt;GetData&lt;Trilean&gt;().Set(1);\n" +
	"    // Puis on affiche le résultat d'une opération\n" +
	"    std::cout &lt;&lt; (!(v1 &amp;&amp; v2 || v3)) &lt;&lt; std::endl;\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<blockquote>\n" +
	"<p><strong><em>Remarque</em></strong> : Si vous souahitez utiliser les fonctions de sauvegarde de la mémoire il est important de surcharger les méthodes <strong>Save</strong> et <strong>Load</strong> de la classe <a href=\"#litescript__type\">Type</a>.</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["exécuter-un-script"] = "" + 
	"<h1 id=\"exécuter-un-script\">Exécuter un script</h1>\n" +
	"<hr />\n" +
	"<h2 id=\"un-script-simple\">Un script simple</h2>\n" +
	"<p>Voici le code source minimum pour exécuter un script :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Contenu du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state(memory);\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Script script(state);\n" +
	"    // Exécution du code\n" +
	"    script.Execute(code);\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<hr />\n" +
	"<h2 id=\"un-script-assembleur-simple\">Un script assembleur simple</h2>\n" +
	"<p>Voici le code source minimum pour exécuter un script assembleur (quasiment la même chose) :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Contenu du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state(memory);\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Assembly script(state);\n" +
	"    // Exécution du code\n" +
	"    script.Execute(code);\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<hr />\n" +
	"<h2 id=\"exécution-instruction-par-instruction\">Exécution instruction par instruction</h2>\n" +
	"<p>Voici le code source minimum pour exécuter un script instruction par instruction :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Contenu du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state(memory);\n" +
	"    // Récupération de la liste d'instruction (même chose pour la classe Assembly)\n" +
	"    std::vector&lt;Instruction&gt; instructions = Script::GetInstructionList(code);\n" +
	"    // Ajout des instructions dans l'état du script\n" +
	"    state.AddInstructions(instructions);\n" +
	"    // Exécution des instructions une par une\n" +
	"    while (!state.Finished()) {\n" +
	"        state.ExecuteSingle();\n" +
	"    }\n" +
	"    return 0;\n" +
	"}</code></pre>";

content["1.0"]["fr"]["index"] = "" + 
	"<h1 id=\"litescript---un-petit-moteur-de-script\">LiteScript - Un petit moteur de script</h1>\n" +
	"<p>LiteScript est un petit moteur de script conçu pour les développeurs qui veulent inclure un\n" +
	"langage de script dans leur application. Le langage de script est similaire à Javascript\n" +
	"avec de l'orienté objet. L'objectif est de créer une alternative aux moteurs de script\n" +
	"actuels et d'intégrer des fonctionnalités qui n'y sont pas présentes.</p>\n" +
	"<p><strong>Fonctionnalités de l'API</strong></p>\n" +
	"<ul>\n" +
	"<li>Lecture de script textuel semblable au Javascript au format UTF-8 (décrit dans cette documentation)</li>\n" +
	"<li>Réduction d'un script en instructions élémentaires (bytecode)</li>\n" +
	"<li>Lecture de script textuel semblable à de l'assembleur correspondant aux instructions élémentaires</li>\n" +
	"<li>Récupération des erreurs de syntaxe lors de la compilation d'un script</li>\n" +
	"<li>Exécution complète ou instruction par instruction d'un script</li>\n" +
	"<li>Gestion dynamique de la mémoire + ramasse-miettes (garbage collector)</li>\n" +
	"<li>Différents types de variables + possibilité d'en créer de nouveaux</li>\n" +
	"<li>Appel de fonctions interne par un script et inversement</li>\n" +
	"<li>Sauvegarde des instructions dans un format textuel semblable à de l'assembleur</li>\n" +
	"<li>Sauvegarde et chargement des instructions, de la mémoire ou de l'état d'un script</li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"vue-densemble-du-langage\">VUE D'ENSEMBLE DU LANGAGE</h2>\n" +
	"<ul>\n" +
	"<li><a href=\"#petit-tour-rapide\">Petit tour rapide</a></li>\n" +
	"<li><a href=\"#les-variables\">Les variables</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#déclarer-une-variable\">Déclarer une variable</a></li>\n" +
	"<li><a href=\"#les-types-de-variables\">Les types de variables</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#les-opérateurs\">Les opérateurs</a></li>\n" +
	"<li><a href=\"#les-structures-de-contrôle\">Les structures de contrôle</a></li>\n" +
	"<li><a href=\"#les-fonctions\">Les fonctions</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#déclaration-dune-fonction\">Déclaration d'une fonction</a></li>\n" +
	"<li><a href=\"#définition-des-arguments\">Définition des arguments</a></li>\n" +
	"<li><a href=\"#instance-et-retour-de-fonction\">Instance et retour de fonction</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#les-classes\">Les classes</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#déclaration-dune-classe\">Déclaration d'une classe</a></li>\n" +
	"<li><a href=\"#héritage-de-classe\">Héritage de classe</a></li>\n" +
	"<li><a href=\"#constructeur-de-classe\">Constructeur de classe</a></li>\n" +
	"<li><a href=\"#les-attributs-et-méthodes\">Les attributs et méthodes</a></li>\n" +
	"<li><a href=\"#les-opérateurs-surchargés\">Les opérateurs surchargés</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#les-espaces-de-noms\">Les espaces de noms</a></li>\n" +
	"</ul>\n" +
	"<h2 id=\"integration-de-lapi\">INTEGRATION DE L'API</h2>\n" +
	"<ul>\n" +
	"<li><a href=\"#ajouter-lapi-dans-un-projet\">Ajouter l'API dans un projet</a></li>\n" +
	"<li><a href=\"#exécuter-un-script\">Exécuter un script</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#un-script-simple\">Un script simple</a></li>\n" +
	"<li><a href=\"#un-script-assembleur-simple\">Un script assembleur simple</a></li>\n" +
	"<li><a href=\"#exécution-instruction-par-instruction\">Exécution instruction par instruction</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#récupérer-les-erreurs-de-syntaxe\">Récupérer les erreurs de syntaxe</a></li>\n" +
	"<li><a href=\"#ajouter-des-variables-au-script\">Ajouter des variables au script</a></li>\n" +
	"<li><a href=\"#créer-un-type-de-variable\">Créer un type de variable</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#un-nouveau-type-de-base\">Un nouveau type de base</a></li>\n" +
	"<li><a href=\"#création-dun-booléen-à-trois-états\">Création d'un booléen à trois états</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#utiliser-le-ramasse-miettes\">Utiliser le ramasse-miettes</a></li>\n" +
	"<li><a href=\"#sauvegarder-dans-des-fichiers\">Sauvegarder dans des fichiers</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#sauvegarder-les-instructions\">Sauvegarder les instructions</a></li>\n" +
	"<li><a href=\"#sauvegarder-la-mémoire\">Sauvegarder la mémoire</a></li>\n" +
	"<li><a href=\"#sauvegarder-létat-du-script\">Sauvegarder l'état du script</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#les-fonctions-utiles\">Les fonctions utiles</a></li>\n" +
	"</ul>\n" +
	"<h2 id=\"les-classes-utiles\">LES CLASSES UTILES</h2>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__object\">LiteScript::Object</a></li>\n" +
	"<li><a href=\"#litescript__type\">LiteScript::Type</a></li>\n" +
	"<li><a href=\"#litescript__variable\">LiteScript::Variable</a></li>\n" +
	"<li><a href=\"#litescript__memory\">LiteScript::Memory</a></li>\n" +
	"<li><a href=\"#litescript__instruction\">LiteScript::Instruction</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-structure-instruction\">La structure Instruction</a></li>\n" +
	"<li><a href=\"#les-instructions-élémentaires\">Les instructions élémentaires</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#litescript__state\">LiteScript::State</a></li>\n" +
	"<li><a href=\"#litescript__assembly\">LiteScript::Assembly</a></li>\n" +
	"<li><a href=\"#litescript__script\">LiteScript::Script</a></li>\n" +
	"</ul>\n" +
	"<h2 id=\"les-classes-complementaires\">LES CLASSES COMPLEMENTAIRES</h2>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-nullable\">La classe &quot;Nullable&quot;</a></li>\n" +
	"<li><a href=\"#les-classes-de-flux-de-données\">Les classes de flux de données</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-ostreamer\">La classe &quot;OStreamer&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-istreamer\">La classe &quot;IStreamer&quot;</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#les-types-de-données\">Les types de données</a>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-number\">La classe &quot;Number&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-string\">La classe &quot;String&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-character\">La classe &quot;Character&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-callback\">La classe &quot;Callback&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-array\">La classe &quot;Array&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-class\">La classe &quot;Class&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-classobject\">La classe &quot;ClassObject&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-namespace\">La classe &quot;Namespace&quot;</a></li>\n" +
	"</ul></li>\n" +
	"<li><a href=\"#la-classe-namer\">La classe &quot;Namer&quot;</a></li>\n" +
	"<li><a href=\"#la-classe-stateexecutor\">La classe &quot;StateExecutor&quot;</a></li>\n" +
	"</ul>";

content["1.0"]["fr"]["la-classe-namer"] = "" + 
	"<h1 id=\"la-classe-namer\">La classe &quot;Namer&quot;</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichier source</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/namer.hpp\">namer.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/namer.cpp\">namer.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>La classe <strong>Namer</strong> permet de gérer des espaces de noms complets pour la classe <a href=\"#litescript__state\">State</a>.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>global</strong> :\n" +
	"Espace de noms principal</p>\n" +
	"<p><a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>current</strong> :\n" +
	"Espace de noms global courant</p>\n" +
	"<p>std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>heap</strong> :\n" +
	"Pile des espaces de noms intermédiaires</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Namer</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>global</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>global</strong> : Espace de noms principal</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Namer</strong>(<strong>const</strong> <a href=\"#la-classe-namer\">Namer</a>&amp; <strong>n</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>n</strong> : Espace de noms à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>Use</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>nsp</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Change l'espace de noms global courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>nsp</strong> : Espace de noms à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Push</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>nsp</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un nouvel espace dans la pile intermédiaire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>nsp</strong> : Espace de noms à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Pop</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>nsp</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime un espace de noms de la pile intermédiaire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>nsp</strong> : Espace de noms à supprimer</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>Get</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne une variable des espaces de noms</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Declare</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Déclare une variable dans l'espace de noms au sommet de la pile ou dans celui courant</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>GarbageCollector</strong>(bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(unsigned int)) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Protège les variables contenus dans les espaces de noms de l'algorithme ramasse-miettes</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>caller</strong> : Fonction prenant en paramètre l'identifiant de l'objet à protéger (retourne vrai si déjà protégé)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>Save</strong>(std::ostream&amp; <strong>stream</strong>, <a href=\"#la-classe-namer\">Namer</a>&amp; <strong>namer</strong>, bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(std::ostream&amp;, unsigned int))</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde le contenu des espaces de noms dans un flux de données binaire</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"<li><strong>namer</strong> : La classe Namer à sauvegarder</li>\n" +
	"<li><strong>caller</strong> : Fonction permettant de sauvegarder une variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <a href=\"#litescript__namer\">Namer</a> <strong>Load</strong>(std::istream&amp; <strong>stream</strong>, <a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, unsigned int (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(std::istream&amp;))</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge le contenu des espaces de noms dans un flux de données binaire et le retourne</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"<li><strong>caller</strong> : Fonction permettant de charger une variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>SaveIDs</strong>(std::ostream&amp; <strong>stream</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde les identifiants des espaces de noms dans un flux de données binaire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>LoadIDs</strong>(std::istream&amp; <strong>stream</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge les identifiants des espaces de noms dans un flux de données binaire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-classe-namer\">Namer</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-namer\">Namer</a>&amp; <strong>n</strong>)</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["la-classe-nullable"] = "" + 
	"<h1 id=\"la-classe-nullable\">La classe &quot;Nullable&quot;</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/nullable.hpp\">nullable.hpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Cette classe permettait d'imiter une caractéristique propre à certains langages de programmation comme le Java, à savoir pouvoir nullifier une variable.</p>\n" +
	"<p>Exemple du Java :</p>\n" +
	"<pre><code class=\"java\">MaClass a = null;\n" +
	"if (a == null)\n" +
	"    a = new MaClass();</code></pre>\n" +
	"<p>Grâce à cette classe, on peut réaliser à peut la même chose en C++ :</p>\n" +
	"<pre><code class=\"cpp\">Nullable&lt;MaClass&gt; a;\n" +
	"if (a.isNull)\n" +
	"    a = MaClass();</code></pre>\n" +
	"<p><strong>Template</strong></p>\n" +
	"<blockquote>\n" +
	"<p>class <strong><em>T</em></strong></p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>char <strong>data</strong>[<strong>sizeof</strong>(<strong><em>T</em></strong>)] :\n" +
	"Données allouées pour l'objet</p>\n" +
	"<p>bool <strong>is_null</strong> :\n" +
	"Indique si l'objet a été créé ou non</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>cont</strong> bool&amp; <strong>isNull</strong> :\n" +
	"Indique si l'objet a été créé ou non</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs/Destructeur</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Nullable</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Nullable</strong>(<strong>const</strong> <strong><em>T</em></strong>&amp; <strong>t</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>t</strong> : Objet à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Nullable</strong>(<strong>const</strong> <a href=\"#la-classe-nullable\">Nullable</a>&lt;<strong><em>T</em></strong>&gt;&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Objet nullable à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>~Nullable</strong>()</li>\n" +
	"</ul>\n" +
	"<p><strong>Méthode publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p>void <strong>Nullify</strong>()</p>\n" +
	"<p>Nullifie l'objet</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"la-classe-nullable\">Nullable</a>&amp;lt<strong><em>T</em></strong>&gt;&amp; <strong>operator=</strong>(<strong>const</strong> <strong><em>T</em></strong>&amp; <strong>t</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Assigne l'objet</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>t</strong> : Objet à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-nullable\">Nullable</a>&lt;<strong><em>T</em></strong>&gt;&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-nullable\">Nullable</a>&lt;<strong><em>T</em></strong>&gt;&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Assigne un objet nullable</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Objet nullable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>operator</strong> <strong><em>T</em></strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'objet</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong><em>T</em></strong>&amp; <strong>operator*</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur vers l'objet</p>\n" +
	"</blockquote>\n" +
	"<p><strong>const</strong> <strong><em>T</em></strong>&amp; <strong>operator*</strong>() <strong>const</strong></p>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur constant vers l'objet</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong><em>T</em></strong>&amp; <strong>operator-&gt;</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur vers l'objet</p>\n" +
	"</blockquote>\n" +
	"<p><strong>const</strong> <strong><em>T</em></strong>&amp; <strong>operator-&gt;</strong>() <strong>const</strong></p>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur constant vers l'objet</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["la-classe-stateexecutor"] = "" + 
	"<h1 id=\"la-classe-stateexecutor\">La classe &quot;StateExecutor&quot;</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/executor.hpp\">executor.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/executor.cpp\">executor.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Cette classe permet à l'état du script d'exécuter chaque instruction élémentaire rapidement. C'est une classe amie de la classe <a href=\"#litescript__state\">State</a>.</p>\n" +
	"<p><strong>Attribut privé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::array&lt;void(*)(<a href=\"#litescript__state\">State</a>&amp;, <a href=\"#la-classe-instruction\">Instruction</a>&amp;), <a href=\"#les-instructions-élémentaires\">InstrCode</a>::<strong>INSTR_NUMBER</strong>&gt; <strong>ARRAY</strong> :\n" +
	"Liste des fonctions à exécuter pour chaque instruction</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthode publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> void <strong>Execute</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, <a href=\"#la-classe-instruction\">Instruction</a>&amp; <strong>instr</strong>) :</p>\n" +
	"<p>Exécute une instruction élémentaire</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"<li><strong>instr</strong> : Instruction à exécuter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes privées</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>PopValue</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>st</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime une variable de la pile d'état avec certaines spécificitées</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>st</strong> : Etat du script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p>De plus, il y a une méthode privée qui correspond à chaque instruction élémentaire, leur prototype est le suivant :</p>\n" +
	"<pre><code class=\"cpp\">static void I_NOM_INSTRUCTION(State&amp;, Instruction&amp;);</code></pre>\n" +
	"<p>Elles prennent en paramètre l'état du script et l'instruction. Le nom de la méthode est directement liée à l'énumération de l'instruction, par exemple, pour l'instruction <strong>INSTR_VALUE_NUMBER</strong>, le nom de la méthode qui l'exécute est <strong>I_VALUE_NUMBER</strong>.</p>";

content["1.0"]["fr"]["les-classes-de-flux-de-données"] = "" + 
	"<h1 id=\"les-classes-de-flux-de-données\">Les classes de flux de données</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichier source</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/streamer.hpp\">streamer.hpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Ces classes permettent la lecture et l'écriture de données dans un flux plus facile en utilisant un format &quot;<strong><em>little endian</em></strong>&quot;.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-ostreamer\">La classe &quot;OStreamer&quot;</h2>\n" +
	"<p>La classe <strong>OStreamer</strong> facilite l'écriture des données dans un flux.</p>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::ostream&amp; <strong>stream</strong> :\n" +
	"Flux de données en sortie</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>OStreamer</strong>(std::ostream&amp; <strong>s</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>s</strong> : Flux de données en sortie</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>OStreamer</strong>(<strong>const</strong> <a href=\"#la-classe-ostreamer\">OStreamer</a>&amp; <strong>s</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>s</strong> : Classe à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthode publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>inline</strong> <strong>static</strong> void <strong>Write</strong>&lt;<strong><em>T</em></strong>&gt;(std::ostream&amp; <strong>stream</strong>, <strong><em>T</em></strong> <strong>v</strong>)</p>\n" +
	"<p>Ecrit une donnée dans le flux</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"<li><strong>v</strong> : Donnée à écrire</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque</em></strong> : l'écriture en little endian n'est géré que pour les types <em>(unsigned) char</em>, <em>(unsigned) short</em>, <em>(unsigned) int</em> et float, l'écriture de tout autre type de donnée dont la taille dépasse un octet ne peut pas être garantie.</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-classe-ostreamer\">OStreamer</a>&amp; operator&lt;&lt;(const <strong><em>T</em></strong>&amp; <strong>v</strong>)</p>\n" +
	"<p>Ecrit une donnée dans le flux et retourne la classe <strong>OStreamer</strong></p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Donnée à écrire</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque</em></strong> : l'écriture en little endian n'est géré que pour les types <em>(unsigned) char</em>, <em>(unsigned) short</em>, <em>(unsigned) int</em> et float, l'écriture de tout autre type de donnée dont la taille dépasse un octet ne peut pas être garantie.</p>\n" +
	"</blockquote>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-istreamer\">La classe &quot;IStreamer&quot;</h2>\n" +
	"<p>La classe <strong>IStreamer</strong> facilite la lecture des données d'un flux.</p>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::istream&amp; <strong>stream</strong> :\n" +
	"Flux de données en entrée</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>IStreamer</strong>(std::istream&amp; <strong>s</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>s</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>IStreamer</strong>(<strong>const</strong> <a href=\"#la-classe-istreamer\">IStreamer</a>&amp; <strong>s</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>s</strong> : Classe à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthode publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>inline</strong> <strong>static</strong> T <strong>Read</strong>&lt;<strong><em>T</em></strong>&gt;(std::ostream&amp; <strong>stream</strong>)</p>\n" +
	"<p>Lit une donnée dans le flux et la retourne</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque</em></strong> : la lecture en little endian n'est géré que pour les types <em>(unsigned) char</em>, <em>(unsigned) short</em>, <em>(unsigned) int</em> et float, la lecture de tout autre type de donnée dont la taille dépasse un octet ne peut pas être garantie.</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-classe-istreamer\">IStreamer</a>&amp; operator&gt;&gt;(const <strong><em>T</em></strong>&amp; <strong>v</strong>)</p>\n" +
	"<p>Lit une donnée dans le flux et retourne la classe <strong>IStreamer</strong></p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Donnée à lire</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque</em></strong> : la lecture en little endian n'est géré que pour les types <em>(unsigned) char</em>, <em>(unsigned) short</em>, <em>(unsigned) int</em> et float, la lecture de tout autre type de donnée dont la taille dépasse un octet ne peut pas être garantie.</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["les-classes"] = "" + 
	"<h1 id=\"les-classes\">Les classes</h1>\n" +
	"<p>Cette page décrit l'ensemble des informations relatives aux classes dans un script.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"déclaration-dune-classe\">Déclaration d'une classe</h2>\n" +
	"<p>La déclaration d'une classe se fait comme suit :</p>\n" +
	"<pre><code class=\"litescript\">class MaClass : classHeritees {\n" +
	"\n" +
	"    // Attributs\n" +
	"    var a, b, c;\n" +
	"\n" +
	"    // Constructeur\n" +
	"    function MaClass(a, b, c) {\n" +
	"        this.classHeritees();\n" +
	"        this.a = a;\n" +
	"        this.b = b;\n" +
	"        this.c = c;\n" +
	"    }\n" +
	"\n" +
	"    // Une méthode\n" +
	"    function maMethode() {\n" +
	"        print(this.a);\n" +
	"        print(this.b);\n" +
	"        print(this.c);\n" +
	"    }\n" +
	"\n" +
	"    // Un opérateur surchargé\n" +
	"    operator add(b) {\n" +
	"        return new MaClass(\n" +
	"            this.a + b.a,\n" +
	"            this.b + b.b,\n" +
	"            this.c + b.c\n" +
	"        );\n" +
	"    }\n" +
	"\n" +
	"}</code></pre>\n" +
	"<p>Il est aussi possible de déclarer une classe anonyme pour la placer dans une variable ou l'appeler directement :</p>\n" +
	"<pre><code class=\"litescript\">// Déclaration d'une classe anonyme\n" +
	"var MaClass = class {\n" +
	"    // Attribut\n" +
	"    var a;\n" +
	"    // Dans le cas d'une classe anonyme, une méthode avec le\n" +
	"    // nom \"constructor\" devient le constructeur\n" +
	"    function constructor() {\n" +
	"        /* INSTRUCTIONS */\n" +
	"    }\n" +
	"}\n" +
	"\n" +
	"// Instanciation directe d'une classe anonyme\n" +
	"var instance = new (class {\n" +
	"    // Attribut\n" +
	"    var a;\n" +
	"    // Constructeur\n" +
	"    function constructor() {\n" +
	"        /* INSTRUCTIONS */\n" +
	"    }\n" +
	"})();</code></pre>\n" +
	"<p>Une classe peut hériter d'autres classes et donc de leurs éléments, contenir des attributs et des méthodes statiques ou non, des classes, elle peut définir un constructeur et surcharger certains opérateurs.</p>\n" +
	"<p><strong><em>Remarque</em></strong> : A la différence de la plupart des langages, il n'y a pas de concept &quot;<em>publique</em>&quot;, &quot;<em>protégé</em>&quot; ou &quot;<em>privé</em>&quot;.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"héritage-de-classe\">Héritage de classe</h2>\n" +
	"<p>Une classe peut hériter d'une ou plusieurs classes, qu'elle soit anonyme ou non, cela se fait de la façon suivante :</p>\n" +
	"<pre><code class=\"litescript\">// La classe MaClass hérite des classes a, b, c\n" +
	"class MaClass : a, b, c {\n" +
	"    /* CONTENU */\n" +
	"}\n" +
	"\n" +
	"// Il est possible d'avoir une expression complexe\n" +
	"// dans l'héritage tant que le résultat est une classe\n" +
	"class MaClass : a + b, getClass(\"personne\") {\n" +
	"    /* CONTENU */\n" +
	"}</code></pre>\n" +
	"<hr />\n" +
	"<h2 id=\"constructeur-de-classe\">Constructeur de classe</h2>\n" +
	"<p>Il est possible de définir un constructeur pour une classe qui sera appelé à la création d'une instance de celle-ci. Ce constructeur est une méthode qui porte le nom de la classe dans le cas où elle n'est pas anonyme et le nom <strong>constructor</strong> dans le cas où elle est anonyme.</p>\n" +
	"<pre><code class=\"litescript\">// Cas d'une classe non anonyme\n" +
	"class MaClass {\n" +
	"\n" +
	"    // Constructeur\n" +
	"    function MaClass() {\n" +
	"        /* INSTRUCTIONS */\n" +
	"    }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// Cas d'une classe anonyme\n" +
	"var MaClass = class {\n" +
	"\n" +
	"    // Constructeur\n" +
	"    function constructor() {\n" +
	"        /* INSTRUCTIONS */\n" +
	"    }\n" +
	"\n" +
	"}</code></pre>\n" +
	"<p>Si leur méthodes n'ont pas été surchargée, il est aussi possible d'appeler les constructeurs et méthodes des classes héritées.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"les-attributs-et-méthodes\">Les attributs et méthodes</h2>\n" +
	"<p>Les attributs et méthodes d'une classe peuvent être définis comme leur déclaration classique :</p>\n" +
	"<pre><code class=\"litescript\">class MaClass {\n" +
	"\n" +
	"    // Attributs\n" +
	"    var a = 2,\n" +
	"        b = \"hello\",\n" +
	"        c = maFonction;\n" +
	"\n" +
	"    // Méthodes\n" +
	"    function f1() {\n" +
	"        return 1;\n" +
	"    }\n" +
	"    function fx(x) {\n" +
	"        return x;\n" +
	"    }\n" +
	"\n" +
	"}</code></pre>\n" +
	"<p>Précédé du mot-clé <strong>static</strong>, ils seront définis comme étant statiques à la classe, c'est à dire qu'ils seront accessibles en dehors d'une instance de la classe.</p>\n" +
	"<pre><code class=\"litescript\">class MaClass {\n" +
	"\n" +
	"    // Attribut statique\n" +
	"    static var i = 0;\n" +
	"\n" +
	"    function MaClass() {\n" +
	"        MaClass.i++;\n" +
	"        // MaClass. avant le i++ n'est pas nécessaire car la\n" +
	"        // méthode se trouve dans la déclaration de la classe\n" +
	"    }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// i est égal à 0\n" +
	"var a = new MaClass();\n" +
	"// i est égal à 1</code></pre>\n" +
	"<p>Il est aussi possible de déclarer une classe dans une autre :</p>\n" +
	"<pre><code class=\"litescript\">class MaClass {\n" +
	"\n" +
	"    class MonAutreClass {\n" +
	"        /* CONTENU */\n" +
	"    }\n" +
	"\n" +
	"    /* CONTENU */\n" +
	"\n" +
	"}</code></pre>\n" +
	"<p>Il ne peut pas y avoir de mot-clé <strong>static</strong> devant une déclaration de classe car elle est automatiquement statique.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"les-opérateurs-surchargés\">Les opérateurs surchargés</h2>\n" +
	"<p>Il est possible de surcharger un opérateur à l'aide du mot-clé <strong>operator</strong> qui sera appelé lors de l'opération indiquée avec l'élément :</p>\n" +
	"<pre><code class=\"litescript\">class Point {\n" +
	"\n" +
	"    // Attributs\n" +
	"    var x = 0,\n" +
	"        y = 0;\n" +
	"\n" +
	"    // Constructeur\n" +
	"    function Point(x, y) {\n" +
	"        if (x != undefined)\n" +
	"            this.x = x;\n" +
	"        if (y != undefined)\n" +
	"            this.y = y;\n" +
	"    }\n" +
	"\n" +
	"    // Surcharge de l'opérateur +\n" +
	"    operator add(p) {\n" +
	"        if (p.x == undefined || p.y == undefined)\n" +
	"            return null;\n" +
	"        else\n" +
	"            return new Point(this.x + p.x, this.y + p.y);\n" +
	"    }\n" +
	"\n" +
	"}\n" +
	"\n" +
	"// La variable c contiendra un point de coordonnées (9, 4)\n" +
	"var a = new Point(3),\n" +
	"    b = new Point(6, 4),\n" +
	"    c = a + b;\n" +
	"\n" +
	"// ATTENTION, si la première opérande n'est pas de la classe Point,\n" +
	"// l'opérateur surchargé ne sera pas appelé. Exemple :\n" +
	"var d = 5,\n" +
	"    e = new Point(4),\n" +
	"    f = d + e;\n" +
	"// L'opérateur appelé est celui de d et pas e</code></pre>\n" +
	"<p>La liste des opérateurs et leur nom est disponible sur la page des <a href=\"##les-opérateurs\">opérateurs</a>.</p>";

content["1.0"]["fr"]["les-espaces-de-noms"] = "" + 
	"<h1 id=\"les-espaces-de-noms\">Les espaces de noms</h1>\n" +
	"<p>Il est possible de changer et stocker les variables définies dans un autre espace de noms, pour ce faire voici la méthode :</p>\n" +
	"<pre><code class=\"litescript\">// Entre dans l'espace de noms \"Math\"\n" +
	"namespace Math;\n" +
	"\n" +
	"// La fonction \"add\" s'ajoute à l'espace de noms \"Math\"\n" +
	"function add(a, b) {\n" +
	"    return a + b;\n" +
	"}\n" +
	"\n" +
	"// Retour à l'espace de noms global (par défaut)\n" +
	"namespace global;\n" +
	"\n" +
	"// La variable a contient 5\n" +
	"var a = Math.add(3, 2);</code></pre>";

content["1.0"]["fr"]["les-fonctions-utiles"] = "" + 
	"<h1 id=\"les-fonctions-utiles\">Les fonctions utiles</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Headers</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/api\">api</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/api/types.hpp\">types.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/api\">api</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/api/script.hpp\">script.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Sources</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/api\">api</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/api/types.cpp\">types.cpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/api\">api</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/api/script.cpp\">script.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Ces fonctions permettent de simplifier la création et la déclaration de variables dans un script.</p>\n" +
	"<p>void <strong>NullifyVariable</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Assigne une variable à nulle</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Variable à nullifier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateUndefined</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type non-défini</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable nulle</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, bool <strong>value</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable booléenne</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, int <strong>value</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;nombre&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, float <strong>value</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;nombre&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, const char * <strong>value</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;chaine de caractères&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, <a href=\"#litescript__variable\">Variable</a> (*<strong>value</strong>)(<a href=\"#litescript__state\">State</a>&amp; <strong><em>s</em></strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong><em>args</em></strong>))</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;appel de fonction&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, int count, <strong>const</strong> Variable * <strong>values</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;tableau&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>count</strong> : Nombre de variables</li>\n" +
	"<li><strong>values</strong> : Variables à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, int count, const char <em> keys[], <strong>const</strong> Variable \</em> <strong>values</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;tableau associatif&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>count</strong> : Nombre de variables</li>\n" +
	"<li><strong>keys</strong> : Noms des variables</li>\n" +
	"<li><strong>values</strong> : Variables à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateVariable</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, <a href=\"#la-classe-class\">Class</a>&amp; <strong>value</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;classe&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>value</strong> : Valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateNamespace</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, int <strong>count</strong>, <strong>const</strong> char * <strong>names</strong>[], <strong>const</strong> <a href=\"#litescript__variable\">Variable</a> * <strong>values</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable de type &quot;espace de noms&quot;</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>count</strong> : Nombre de variables</li>\n" +
	"<li><strong>names</strong> : Noms des variables</li>\n" +
	"<li><strong>values</strong> : Variables à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p>unsigned int <strong>AddCallback</strong>(<a href=\"#litescript__variable\">Variable</a> (*<strong>value</strong>)(<a href=\"#litescript__state\">State</a>&amp; <strong>s</strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>))</p>\n" +
	"<blockquote>\n" +
	"<p>Ajoute une fonction interne à la liste des appels et retourne son index</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>value</strong> : Appel de fonction interne</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>CreateCallback</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, unsigned int <strong>id</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Crée une variable contenant un appel de fonction interne de la liste des appels</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"<li><strong>id</strong> : Index dans la liste des appels</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p>void <strong>DeclareVariable</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, <strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</p>\n" +
	"<blockquote>\n" +
	"<p>Déclare une variable dans l'espace de nom courant du script</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["les-fonctions"] = "" + 
	"<h1 id=\"les-fonctions\">Les fonctions</h1>\n" +
	"<p>Cette page décrit l'ensemble des informations relatives aux fonctions dans un script.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"déclaration-dune-fonction\">Déclaration d'une fonction</h2>\n" +
	"<p>La déclaration d'une fonction se fait comme suit :</p>\n" +
	"<pre><code class=\"litescript\">function maFonction(arguments) {\n" +
	"    /* INSTRUCTIONS */\n" +
	"}</code></pre>\n" +
	"<p>Il est aussi possible de déclarer une fonction anonyme pour la placer dans une variable ou l'appeler directement :</p>\n" +
	"<pre><code class=\"litescript\">// Déclaration d'une fonction anonyme\n" +
	"var maFonction = function(arguments) {\n" +
	"    /* INSTRUCTIONS */\n" +
	"}\n" +
	"\n" +
	"// Appel direct d'une fonction anonyme\n" +
	"(function(arguments) {\n" +
	"    /* INSTRUCTIONS */\n" +
	"})();</code></pre>\n" +
	"<p>Une déclaration nécessite tout d'abord le mot-clé <strong>function</strong>, un nom de variable dans le cas où ce n'est pas une fonction anonyme, des arguments et des instructions.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"définition-des-arguments\">Définition des arguments</h2>\n" +
	"<p>Il est possible de déclarer plusieurs arguments (assignables ou non) :</p>\n" +
	"<pre><code class=\"litescript\">function maFonction(a, b, c) {\n" +
	"    print(a);\n" +
	"    print(b);\n" +
	"    print(c);\n" +
	"}\n" +
	"\n" +
	"// Les variables a, b et c seront de type undefined\n" +
	"maFonction();\n" +
	"// Les variables a et b seront respectivement égales à 3 et \"hello\"\n" +
	"// et la variable c sera de type undefined\n" +
	"maFonction(3, \"hello\");</code></pre>\n" +
	"<p>Mais il est possible de récupérer l'ensemble des arguments envoyés :</p>\n" +
	"<pre><code class=\"litescript\">// La variable args récupèrera tous les arguments sous forme de tableau\n" +
	"function f1(args...) {\n" +
	"    for (var i = 0; i &lt; args.length; i++)\n" +
	"        print(args[i]);\n" +
	"}\n" +
	"\n" +
	"// La variable c récupèrera tous les arguments à partir du troisième\n" +
	"// sous forme de tableau\n" +
	"function f2(a, b, c...) {\n" +
	"    // Affiche le premier argument\n" +
	"    print(a);\n" +
	"    // Affiche le second argument\n" +
	"    print(b);\n" +
	"    // Affiche le troisième argument\n" +
	"    print(c[0]);\n" +
	"}\n" +
	"\n" +
	"// La variable args contiendra [ 1, 2, 3 ]\n" +
	"f1(1, 2, 3);\n" +
	"// La variable c contiendra [ 3 ]\n" +
	"f2(1, 2, 3);</code></pre>\n" +
	"<hr />\n" +
	"<h2 id=\"instance-et-retour-de-fonction\">Instance et retour de fonction</h2>\n" +
	"<p>Dans le cas d'une fonction contenue dans un tableau ou une méthode contenue dans une instance de classe, l'objet qui les contients se retrouve associé au mot-clé <strong>this</strong> :</p>\n" +
	"<pre><code class=\"litescript\">var tab = [\n" +
	"    3, 2, 4, 1\n" +
	"] + {\n" +
	"    // Fonction de tri\n" +
	"    trier: function() {\n" +
	"        for (var i = 0; i &lt; this.length - 1; i++) {\n" +
	"            var min = i;\n" +
	"            for (var j = i + 1; j &lt; this.length; j++) {\n" +
	"                if (this[j] &lt; this[min])\n" +
	"                    min = j;\n" +
	"            }\n" +
	"            if (min != i) {\n" +
	"                var tmp = this[i];\n" +
	"                this[i] = this[min];\n" +
	"                this[min] = tmp;\n" +
	"            }\n" +
	"        }\n" +
	"    }\n" +
	"}\n" +
	"\n" +
	"// La variable tab contient [ 3, 2, 4, 1 ]\n" +
	"tab.trier();\n" +
	"// La variable tab contient [ 1, 2, 3, 4 ]</code></pre>\n" +
	"<p>Enfin pour retourner une valeur, il faut utiliser le mot-clé <strong>return</strong>, la méthode est la suivante :</p>\n" +
	"<pre><code class=\"litescript\">// Fonction d'addition\n" +
	"function add(a, b) {\n" +
	"    return a + b;\n" +
	"}\n" +
	"\n" +
	"// La variable a contiendra 5\n" +
	"var a = add(2, 3);</code></pre>";

content["1.0"]["fr"]["les-opérateurs"] = "" + 
	"<h1 id=\"les-opérateurs\">Les opérateurs</h1>\n" +
	"<p>Cette page liste l'ensemble des opérateurs disponibles dans un script, leur priorité ansi que leur nom pour la surcharge dans une classe.</p>\n" +
	"<hr />\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Priorité</th>\n" +
	"<th>Nom de l'opérateur</th>\n" +
	"<th>Symbole</th>\n" +
	"<th>Nom de surcharge</th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>1</td>\n" +
	"<td>Post-incrémentation</td>\n" +
	"<td><code>@a++</code></td>\n" +
	"<td><code>post_increment</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>1</td>\n" +
	"<td>Post-décrémentation</td>\n" +
	"<td><code>@a--</code></td>\n" +
	"<td><code>post_decrement</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>1</td>\n" +
	"<td>Appel de fonction</td>\n" +
	"<td><code>@a()</code></td>\n" +
	"<td><code>call</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>1</td>\n" +
	"<td>Accès de tableau</td>\n" +
	"<td><code>@a[@b]</code></td>\n" +
	"<td><code>get</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>1</td>\n" +
	"<td>Accès de membre</td>\n" +
	"<td><code>@a.@b</code></td>\n" +
	"<td>-</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Pré-incrémentation</td>\n" +
	"<td><code>++@a</code></td>\n" +
	"<td><code>pre_increment</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Pré-décrémentation</td>\n" +
	"<td><code>--@a</code></td>\n" +
	"<td><code>pre_decrement</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Copie</td>\n" +
	"<td><code>+@a</code></td>\n" +
	"<td><code>unary_plus</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Opposition</td>\n" +
	"<td><code>-@a</code></td>\n" +
	"<td><code>unary_minus</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Négation</td>\n" +
	"<td><code>!@a</code></td>\n" +
	"<td><code>not</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Négation binaire</td>\n" +
	"<td><code>~@a</code></td>\n" +
	"<td><code>bitwise_not</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>2</td>\n" +
	"<td>Instanciation</td>\n" +
	"<td><code>new @a()</code></td>\n" +
	"<td>-</td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>3</td>\n" +
	"<td>Multiplication</td>\n" +
	"<td><code>@a * @b</code></td>\n" +
	"<td><code>multiply</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>3</td>\n" +
	"<td>Division</td>\n" +
	"<td><code>@a / @b</code></td>\n" +
	"<td><code>divide</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>3</td>\n" +
	"<td>Modulo</td>\n" +
	"<td><code>@a % @b</code></td>\n" +
	"<td><code>modulo</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>4</td>\n" +
	"<td>Addition</td>\n" +
	"<td><code>@a + @b</code></td>\n" +
	"<td><code>add</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>4</td>\n" +
	"<td>Soustraction</td>\n" +
	"<td><code>@a - @b</code></td>\n" +
	"<td><code>subtract</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>5</td>\n" +
	"<td>Décalage à gauche</td>\n" +
	"<td><code>@a &lt;&lt; @b</code></td>\n" +
	"<td><code>left_shift</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>5</td>\n" +
	"<td>Décalage à droite</td>\n" +
	"<td><code>@a &gt;&gt; @b</code></td>\n" +
	"<td><code>right_shift</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>6</td>\n" +
	"<td>Supériorité</td>\n" +
	"<td><code>@a &gt; @b</code></td>\n" +
	"<td><code>greater</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>6</td>\n" +
	"<td>Infériorité</td>\n" +
	"<td><code>@a &lt; @b</code></td>\n" +
	"<td><code>lesser</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>6</td>\n" +
	"<td>Supériorité ou égalité</td>\n" +
	"<td><code>@a &gt;= @b</code></td>\n" +
	"<td><code>greater_equal</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>6</td>\n" +
	"<td>Infériorité ou égalité</td>\n" +
	"<td><code>@a &lt;= @b</code></td>\n" +
	"<td><code>lesser_equal</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>7</td>\n" +
	"<td>Egualité</td>\n" +
	"<td><code>@a == @b</code></td>\n" +
	"<td><code>equal</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>7</td>\n" +
	"<td>Inégualité</td>\n" +
	"<td><code>@a != @b</code></td>\n" +
	"<td><code>not_equal</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>8</td>\n" +
	"<td>Intersection binaire</td>\n" +
	"<td><code>@a &amp; @b</code></td>\n" +
	"<td><code>bitwise_and</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>9</td>\n" +
	"<td>Union exclusif binaire</td>\n" +
	"<td><code>@a ^ @b</code></td>\n" +
	"<td><code>bitwise_xor</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>10</td>\n" +
	"<td>Union binaire</td>\n" +
	"<td><code>@a | @b</code></td>\n" +
	"<td><code>bitwise_or</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>11</td>\n" +
	"<td>Intersection</td>\n" +
	"<td><code>@a &amp;&amp; @b</code></td>\n" +
	"<td><code>and</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>12</td>\n" +
	"<td>Union</td>\n" +
	"<td><code>@a || @b</code></td>\n" +
	"<td><code>or</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>&nbsp;</td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"<td></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>13</td>\n" +
	"<td>Assignation</td>\n" +
	"<td><code>@a = @b</code></td>\n" +
	"<td><code>assign</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>13</td>\n" +
	"<td>Addition et assignation</td>\n" +
	"<td><code>@a += @b</code></td>\n" +
	"<td><code>add_assign</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>13</td>\n" +
	"<td>Soustraction et assignation</td>\n" +
	"<td><code>@a -= @b</code></td>\n" +
	"<td><code>subtract_assign</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>13</td>\n" +
	"<td>Multiplication et assignation</td>\n" +
	"<td><code>@a *= @b</code></td>\n" +
	"<td><code>multiply_assign</code></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td>13</td>\n" +
	"<td>Division et assignation</td>\n" +
	"<td><code>@a /= @b</code></td>\n" +
	"<td><code>divide_assign</code></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Les opérateurs sont employés dans une expression comme celle-ci :</p>\n" +
	"<pre><code class=\"litescript\">a = b * c + 6 - fibo(5);</code></pre>";

content["1.0"]["fr"]["les-structures-de-contrôle"] = "" + 
	"<h1 id=\"les-structures-de-contrôle\">Les structures de contrôle</h1>\n" +
	"<p>Cette page liste les structures de contrôle disponibles dans le langage de script. Elle sont identiques aux structures Javascript.</p>\n" +
	"<hr />\n" +
	"<p><strong>Si .. alors .. sinon</strong></p>\n" +
	"<pre><code class=\"litescript\">// Structure sans le \"sinon\"\n" +
	"if (condition) {\n" +
	"    /* ALORS */\n" +
	"}\n" +
	"\n" +
	"// Structure avec le \"sinon\"\n" +
	"if (condition) {\n" +
	"    /* ALORS */\n" +
	"}\n" +
	"else {\n" +
	"    /* SINON */\n" +
	"}</code></pre>\n" +
	"<p><strong>La boucle FOR</strong></p>\n" +
	"<pre><code class=\"litescript\">for (initialisation, condition, iteration) {\n" +
	"    /* TRAITEMENT */\n" +
	"}</code></pre>\n" +
	"<p><strong>La boucle WHILE</strong></p>\n" +
	"<pre><code class=\"litescript\">while (condition) {\n" +
	"    /* TRAITEMENT */\n" +
	"}</code></pre>\n" +
	"<p><strong>La boucle DO .. WHILE</strong></p>\n" +
	"<pre><code class=\"litescript\">do {\n" +
	"    /* TRAITEMENT */\n" +
	"} while (condition);</code></pre>\n" +
	"<p>Il est possible d'omettre les accolades (&quot;{&quot; et &quot;}&quot;) dans le traitement, dans ce cas, il ne pourra y avoir qu'une seule instruction.</p>\n" +
	"<p><strong>La structure SWITCH</strong></p>\n" +
	"<pre><code class=\"litescript\">switch (valeur) {\n" +
	"    case 1:\n" +
	"    case 2:\n" +
	"        /* TRAITEMENT */\n" +
	"        break;\n" +
	"    case f1():\n" +
	"    case 3 / 6 + 8 * f2():\n" +
	"        /* TRAITEMENT */\n" +
	"    default:\n" +
	"        /* TRAITEMENT */\n" +
	"}</code></pre>";

content["1.0"]["fr"]["les-types-de-données"] = "" + 
	"<h1 id=\"les-types-de-données\">Les types de données</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/types\">types</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/types/internal.hpp\">internal.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/types\">types</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/types/internal.cpp\">internal.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Les différentes classes héritées de la classe abstraite &quot;<a href=\"#litescript__type\">Type</a>&quot; qui définissent chaque type de variable manipulable dans un script stockent dans l'<a href=\"#litescript__object\">Object</a> conserné une classe qui lui correspond. Ces classes sont décrites ci-dessous.</p>\n" +
	"<p>Les types &quot;<strong>UNDEFINED</strong>&quot;, et &quot;<strong>NIL</strong>&quot; ne stockent aucune donnée et le type &quot;<strong>BOOLEAN</strong>&quot; stocke seulement un booléen (<em>bool</em>).</p>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-number\">La classe &quot;Number&quot;</h2>\n" +
	"<p>Cette classe permet la manipulation de nombres. Elle correspond au type <strong>NUMBER</strong>.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>char <strong>numeric_type</strong> :\n" +
	"Type de représentation numérique (<em>int</em> ou <em>float</em>)</p>\n" +
	"<p>union <strong>value</strong> :\n" +
	"Représentations numériques</p>\n" +
	"<ul>\n" +
	"<li>int <strong>integer</strong></li>\n" +
	"<li>float <strong>floatting</strong></li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Number</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Number</strong>(int <strong>value</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>value</strong> : valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Number</strong>(float <strong>value</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>value</strong> : valeur à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Number</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>number</strong> : Nombre à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>operator</strong> int() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie le nombre en entier</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>operator</strong> float() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie le nombre en flottant</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>operator</strong> std::string() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie le nombre en chaine de caractères</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-number\">Number</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator+</strong>() <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator-</strong>() <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator++</strong>() <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator++</strong>(int) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator--</strong>() <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator--</strong>(int) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator+</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator-</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator*</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator/</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator%</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li>bool <strong>operator==</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator!=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator&lt;</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator&lt;=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator&gt;</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator&gt;=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator~</strong>() <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator&amp;</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator|</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator^</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator&lt;&lt;</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a> <strong>operator&gt;&gt;</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a>&amp; <strong>operator+=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a>&amp; <strong>operator-=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a>&amp; <strong>operator*=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"<li><a href=\"#la-classe-number\">Number</a>&amp; <strong>operator/=</strong>(<strong>const</strong> <a href=\"#la-classe-number\">Number</a>&amp; <strong>number</strong>)</li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-string\">La classe &quot;String&quot;</h2>\n" +
	"<p>Cette classe permet la manipulation de chaines de caractères au format <strong>UTF-8</strong>. Elle correspond au type <strong>STRING</strong>.</p>\n" +
	"<p><strong>Attribut privé</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::u32string <strong>str</strong> :\n" +
	"Contenu de la chaine de caractère au format <strong>UNICODE</strong></p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(char <strong>c</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Caractère (au format <strong>ASCII</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(char32_t <strong>c</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Caractère (au format <strong>UNICODE</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(<strong>const</strong> char * <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UTF-8</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(<strong>const</strong> char32_t * <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UNICODE</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(<strong>const</strong> std::string&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UTF-8</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(<strong>const</strong> std::u32string&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UNICODE</strong>) à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>String</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetLength</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre de caractères contenus dans la chaine</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>std::u32string&amp; <strong>GetData</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le contenu de la chaine de caractères au format <strong>UNICODE</strong></p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> std::u32string&amp; <strong>GetData</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le contenu de la chaine de caractères (constant) au format <strong>UNICODE</strong></p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Clear</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime le contenu de la chaine de caractères</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Insert</strong>(unsigned int <strong>index</strong>, const <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Insère une chaine de caractères dans la chaine courante</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index à partir duquel insérer la chaine</li>\n" +
	"<li><strong>str</strong> : Chaine de caractères à insérer</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Erase</strong>(unsigned int <strong>index</strong>, unsigned int <strong>length</strong> = 1)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime un certain nombre de caractères de la chaine</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Position initiale</li>\n" +
	"<li><strong>length</strong> (<em>facultatif</em>) : Nombre de caractères à supprimer</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Replace</strong>(unsigned int <strong>index</strong>, <strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Remplace un caractère par une chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index du caractère àa remplacer</li>\n" +
	"<li><strong>str</strong> : Chaine de caractères à insérer</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Replace</strong>(unsigned int <strong>index</strong>, unsigned int <strong>length</strong>, <strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Remplace un ensemble de caractères par une chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index du premier caractère à remplacer</li>\n" +
	"<li><strong>length</strong> : Nombre de caractères à remplacer</li>\n" +
	"<li><strong>str</strong> : Chaine de caractères à insérer</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-string\">String</a> <strong>SubString</strong>(unsigned int <strong>index</strong>, unsigned int <strong>length</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un sous-ensemble de la chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Position initiale du sous-ensemble</li>\n" +
	"<li><strong>length</strong> : Nombre de caractères à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::string <strong>ConvertToUTF8</strong>(<strong>const</strong> std::u32string&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie une chaine de caractères du format <strong>UNICODE</strong> au format <strong>UTF-8</strong></p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UNICODE</strong>) à convertir</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::u32string <strong>ConvertToUnicode</strong>(<strong>const</strong> std::string&amp; <strong>str</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertir une chaine de caractères du format <strong>UTF-8</strong> au format <strong>UNICODE</strong></p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>str</strong> : Chaine de caractères (au format <strong>UTF-8</strong>) à convertir</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetChar</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, unsigned int <strong>index</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne une variable référençant un caractère contenu dans la chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale pour allouer la variable</li>\n" +
	"<li><strong>index</strong> : Position du caractère dans la chaine</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetMember</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>, <strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne une variable référençant un membre de la chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale pour allouer la variable</li>\n" +
	"<li><strong>name</strong> : Nom du membre de la chaine</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-string\">String</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"<li><a href=\"#la-classe-string\">String</a> <strong>operator+</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-string\">String</a> <strong>operator*</strong>(unsigned int <strong>x</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator==</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator!=</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator&gt;</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator&lt;</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator&gt;=</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><strong>bool</strong> <strong>operator&lt;=</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#la-classe-string\">String</a>&amp; <strong>operator+=</strong>(<strong>const</strong> <a href=\"#la-classe-string\">String</a>&amp; <strong>str</strong>)</li>\n" +
	"<li><a href=\"#la-classe-string\">String</a>&amp; <strong>operator*=</strong>(unsigned int <strong>x</strong>)</li>\n" +
	"<li>char32_t&amp; <strong>operator[]</strong>(unsigned int <strong>index</strong>)</li>\n" +
	"<li><strong>const</strong> char32_t&amp; <strong>operator[]</strong>(unsigned int <strong>index</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-character\">La classe &quot;Character&quot;</h2>\n" +
	"<p>La classe <strong>Character</strong> est une classe un peu spéciale qui permet de modifier le caractère d'une chaine dans un script. Il est fortement déconseillé de la manipuler, c'est pourquoi elle ne sera pas documentée ici. Vous pouvez toujours regarder comment elle fonctionne dans les fichiers sources.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-callback\">La classe &quot;Callback&quot;</h2>\n" +
	"<p>Cette classe permet de manipuler les appels de fonctions en script ou internes à l'application. Elle correspond au type <strong>CALLBACK</strong>.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>unsigned int <strong>intrl_idx</strong> :\n" +
	"Index de la liste d'instructions dans l'état du script</p>\n" +
	"<p>unsigned int <strong>line_num</strong> :\n" +
	"Index de l'instruction dans la liste</p>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> (* <strong>call_ptr</strong>)(<a href=\"#litescript__state\">State</a>&amp;, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp;) :\n" +
	"Pointeur sur une fonction interne à l'application, si nulle, l'appel se trouve dans le script</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publiques</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::vector&lt;<a href=\"#litescript__variable\">Variable</a> (*)(<a href=\"#litescript__state\">State</a>&amp;, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp;)&gt; <strong>List</strong> :\n" +
	"Liste statique de fonctions internes (nécéssaires pour la sauvegarde/chargement de la mémoire dans un fichier)</p>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"<p><a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>This</strong> :\n" +
	"Variable nullable contenue dans le mot-clé <strong>this</strong></p>\n" +
	"<p><a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#la-classe-namer\">Namer</a>&gt; <strong>nsp</strong> :\n" +
	"Espace de noms de la fonction</p>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>I</strong> :\n" +
	"Index de la liste d'instructions dans l'état du script</p>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>L</strong> :\n" +
	"Index de l'instruction dans la liste</p>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>CallbackIndex</strong> :\n" +
	"Index de la fonction dans la liste statique</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Callback</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Callback</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>s</strong>, unsigned int <strong>i</strong>, unsigned int <strong>l</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>s</strong> : Etat du script</li>\n" +
	"<li><strong>i</strong> : Index de la liste d'instructions dans l'état du script</li>\n" +
	"<li><strong>l</strong> : Index de l'instruction dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Callback</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>, unsigned int <strong>i</strong>, unsigned int <strong>l</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire principale</li>\n" +
	"<li><strong>i</strong> : Index de la liste d'instructions dans l'état du script</li>\n" +
	"<li><strong>l</strong> : Index de l'instruction dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Callback</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>, <a href=\"#litescript__variable\">Variable</a> (* <strong>cptr</strong>)(<a href=\"#litescript__state\">State</a>&amp;, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp;))</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire principale</li>\n" +
	"<li><strong>cptr</strong> : Pointeur sur une fonction interne à l'application</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Callback</strong>(<strong>const</strong> <a href=\"#la-classe-callback\">Callback</a>&amp; <strong>c</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Appel de fonction à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthode publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p>bool <strong>isInternal</strong>() <strong>const</strong></p>\n" +
	"<p>Indique si l'appel de fonction est interne à l'application</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-callback\">Callback</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-callback\">Callback</a>&amp; <strong>c</strong>)</li>\n" +
	"<li>bool <strong>operator==</strong>(<strong>const</strong> <a href=\"#la-classe-callback\">Callback</a>&amp; <strong>c</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator!=</strong>(<strong>const</strong> <a href=\"#la-classe-callback\">Callback</a>&amp; <strong>c</strong>) <strong>const</strong></li>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator()</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>)</li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-array\">La classe &quot;Array&quot;</h2>\n" +
	"<p>Cette classe permet la manipulation de tableaux associatifs. Elle correspond au type <strong>ARRAY</strong>.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::vector&lt;std::pair&lt;std::string, <a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>named</strong> :\n" +
	"Liste des éléments indexés litéralement</p>\n" +
	"<p>std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>unamed</strong> :\n" +
	"Liste des éléments indexés numériquement</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Array</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Array</strong>(<strong>const</strong> <a href=\"#la-classe-array\">Array</a>&amp; <strong>o</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>o</strong> : Tableau associatif à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>Add</strong>(unsigned int <strong>idx</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un élément indexé numériquement</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Add</strong>(<strong>const</strong> char * <strong>idx</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un élément indexé litéralement</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>NamedCount</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre d'éléments indexés litéralement</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>UnamedCount</strong>() const</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre d'éléments indexés numériquement</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> char * <strong>GetNamedKey</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom de l'élément indexés litéralement par rapport à sa position</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetNamedVariable</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'élément indexés litéralement par rapport à sa position</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>int <strong>IndexOfNamed</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'index d'un élément par rapport à son nom</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de l'élément</li>\n" +
	"</ul>\n" +
	"<p>Retourne -1 si l'élément n'existe pas</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>int <strong>ExistNamed</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Indique si un élément indexés litéralement existe dans le tableau ou non</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>int <strong>ExistUnamed</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Indique si un élément indexés numériquement existe dans le tableau ou non</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ConstantGet</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un élément indexés numériquement</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ConstantGet</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un élément indexés litéralement</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-array\">Array</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-array\">Array</a>&amp; <strong>obj</strong>)</li>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator[]</strong>(unsigned int <strong>idx</strong>)</li>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator[]</strong>(<strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-class\">La classe &quot;Class&quot;</h2>\n" +
	"<p>Cette classe permet de manipuler la définition d'une classe dans le script. Elle correspond au type <strong>CLASS</strong>.</p>\n" +
	"<p><strong>Enumérateur</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>OperatorType</strong> :\n" +
	"Type d'opérateur</p>\n" +
	"<ul>\n" +
	"<li><strong>OP_TYPE_ASSIGN</strong> : Assignation</li>\n" +
	"<li><strong>OP_TYPE_UNARY_PLUS</strong> : Copie</li>\n" +
	"<li><strong>OP_TYPE_UNARY_MINUS</strong> : Opposé</li>\n" +
	"<li><strong>OP_TYPE_PRE_INCR</strong> : Pré-incrémentation</li>\n" +
	"<li><strong>OP_TYPE_POST_INCR</strong> : Post-incrémentation</li>\n" +
	"<li><strong>OP_TYPE_PRE_DECR</strong> : Pré-décrémentation</li>\n" +
	"<li><strong>OP_TYPE_POST_DECR</strong> : Post-décrémentation</li>\n" +
	"<li><strong>OP_TYPE_ADD</strong> : Addition</li>\n" +
	"<li><strong>OP_TYPE_SUB</strong> : Soustraction</li>\n" +
	"<li><strong>OP_TYPE_MUL</strong> : Multiplication</li>\n" +
	"<li><strong>OP_TYPE_DIV</strong> : Division</li>\n" +
	"<li><strong>OP_TYPE_MOD</strong> : Modulo</li>\n" +
	"<li><strong>OP_TYPE_EQU</strong> : Egalité</li>\n" +
	"<li><strong>OP_TYPE_DIF</strong> : Inégalité</li>\n" +
	"<li><strong>OP_TYPE_GREAT</strong> : Supériorité</li>\n" +
	"<li><strong>OP_TYPE_LESS</strong> : Infériorité</li>\n" +
	"<li><strong>OP_TYPE_GREAT_EQU</strong> : Supériorité ou égalité</li>\n" +
	"<li><strong>OP_TYPE_LESS_EQU</strong> : Infériorité ou égalité</li>\n" +
	"<li><strong>OP_TYPE_LOG_NOT</strong> : Négation</li>\n" +
	"<li><strong>OP_TYPE_LOG_AND</strong> : Intersection</li>\n" +
	"<li><strong>OP_TYPE_LOG_OR</strong> : Union</li>\n" +
	"<li><strong>OP_TYPE_BIT_NOT</strong> : Négation binaire</li>\n" +
	"<li><strong>OP_TYPE_BIT_AND</strong> : Intersection binaire</li>\n" +
	"<li><strong>OP_TYPE_BIT_OR</strong> : Union binaire</li>\n" +
	"<li><strong>OP_TYPE_BIT_XOR</strong> : Union exclusif binaire</li>\n" +
	"<li><strong>OP_TYPE_LSHIFT</strong> : Décalage à gauche</li>\n" +
	"<li><strong>OP_TYPE_RSHIFT</strong> : Décalage à droite</li>\n" +
	"<li><strong>OP_TYPE_ADD_ASSIGN</strong> : Addition et assignation</li>\n" +
	"<li><strong>OP_TYPE_SUB_ASSIGN</strong> : Soustraction et assignation</li>\n" +
	"<li><strong>OP_TYPE_MUL_ASSIGN</strong> : Multiplication et assignation</li>\n" +
	"<li><strong>OP_TYPE_DIV_ASSIGN</strong> : Division et assignation</li>\n" +
	"<li><strong>OP_TYPE_ARRAY</strong> : Accès</li>\n" +
	"<li><strong>OP_TYPE_CALL</strong> : Appel</li>\n" +
	"<li><strong>OP_TYPE_NUMBER</strong> : Nombre d'opérateurs</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::vector&lt;Variable&gt; <strong>inherit</strong> :\n" +
	"Liste des classes héritées</p>\n" +
	"<p>std::vector&lt;std::pair&lt;std::string, <a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>s_members</strong> :\n" +
	"Liste des membres statiques</p>\n" +
	"<p>std::vector&lt;std::pair&lt;std::string, <a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>us_members</strong> :\n" +
	"Liste des membres non statiques</p>\n" +
	"<p>std::array&lt;<a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt;, <a href=\"#la-classe-class\">OperatorType</a>::<strong>OP_TYPE_NUMBER</strong>&gt; <strong>op_members</strong> :\n" +
	"Liste des opérateurs surchargés</p>\n" +
	"<p>int <strong>constructor_index</strong> :\n" +
	"Index du constructeur dans les membres non statiques (-1 s'il n'existe pas)</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Class</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Class</strong>(<strong>const</strong> <a href=\"#la-classe-class\">Class</a>&amp; <strong>c</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Classe à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>Inherit</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Fait hériter la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Classe à hériter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>bool <strong>DefineConstructor</strong>(<strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Définie le constructeur de la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du constructeur</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddStatic</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un membre statique</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du membre</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddUnstatic</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un membre non statique</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du membre</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddOperator</strong>(<a href=\"#la-classe-class\">OperatorType</a> <strong>op</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un opérateur surchargé</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>op</strong> : Type d'opérateur</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddOperator</strong>(unsigned int <strong>op</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un opérateur surchargé</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>op</strong> : Code du type d'opérateur</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> std::array&lt;<a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt;, <a href=\"#la-classe-class\">OperatorType</a>::<strong>OP_TYPE_NUMBER</strong>&gt;&amp; <strong>GetOperators</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne la liste des opérateurs surchargés</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetStaticMember</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre statique de la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du membre</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetUnstaticMember</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre non statique de la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du membre</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetOperator</strong>(<a href=\"#la-classe-class\">OperatorType</a> <strong>op</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un opérateur de la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>op</strong> : Type d'opérateur</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetOperator</strong>(unsigned int <strong>op</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un opérateur de la classe</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>op</strong> : Code du type d'opérateur</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>CreateElement</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>self</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Crée un élément de la classe</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"<li><strong>args</strong> : Arguments du constructeur</li>\n" +
	"<li><strong>self</strong> : Variable contenant la classe</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>GetInherits</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne la liste des classes héritées</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetStaticCount</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre de membres statiques</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetUnstaticCount</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre de membres non statiques</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>const char * <strong>GetStaticName</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom d'un membre statique</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index du membre dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>const char * <strong>GetUnstaticName</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom d'un membre non statique</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index du membre dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetStaticMember</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre statique</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index du membre dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetUnstaticMember</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre non statique</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index du membre dans la liste</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>int <strong>GetConstructorIndex</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'index du constructeur de la classe</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-class\">Class</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-class\">Class</a>&amp; <strong>c</strong>)</li>\n" +
	"<li>bool <strong>operator==</strong>(<strong>const</strong> <a href=\"#la-classe-class\">Class</a>&amp; <strong>c</strong>) <strong>const</strong></li>\n" +
	"<li>bool <strong>operator!=</strong>(<strong>const</strong> <a href=\"#la-classe-class\">Class</a>&amp; <strong>c</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-classobject\">La classe &quot;ClassObject&quot;</h2>\n" +
	"<p>Cette classe permet de manipuler un objet créé par une classe dans le script. Elle correspond au type <strong>CLASS_OBJECT</strong>.</p>\n" +
	"<p><strong>Attribut privé</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::vector&lt;std::pair&lt;std::string, <a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>members</strong> :\n" +
	"Liste des membres de l'objet</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publiques</strong></p>\n" +
	"<blockquote>\n" +
	"<p>[Nullable]&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>class_base</strong> :\n" +
	"Variable contenant la classe principale</p>\n" +
	"<p><a href=\"#la-classe-class\">Class</a> * <strong>ClassBase</strong> :\n" +
	"Classe de l'élément</p>\n" +
	"<p><a href=\"#litescript__state\">State</a> * <strong>ScriptState</strong> :\n" +
	"Etat du script</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>ClassObject</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>ClassObject</strong>(<strong>const</strong> <a href=\"#la-classe-classobject\">ClassObject</a>&amp; <strong>c</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>c</strong> : Element de classe à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>AddMember</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un membre à l'élément</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de l'élément</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetMemberCount</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre de membres dans l'élément</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> char * <strong>GetMemberName</strong>(unsigned int <strong>index</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom d'un membre de l'élément</p>\n" +
	"<p>Parmètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetMemberVariable</strong>(unsigned int <strong>index</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre de l'élément</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetMember</strong>(<strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre de l'élément</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de l'élément</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-classe-classobject\">ClassObject</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-classe-classobject\">ClassObject</a>&amp; <strong>obj</strong>)</p>\n" +
	"</blockquote>\n" +
	"<hr />\n" +
	"<h2 id=\"la-classe-namespace\">La classe &quot;Namespace&quot;</h2>\n" +
	"<p>Cette classe permet de manipuler un espace de noms. Elle correspond au type <strong>NAMESPACE</strong>.</p>\n" +
	"<p><strong>Attribut privé</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::vector&lt;std::pair&lt;std::string, <a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>vars</strong> :\n" +
	"Liste des variables</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Namespace</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Namespace</strong>(<strong>const</strong> <a href=\"#la-classe-namespace\">Namespace</a>&amp; <strong>nsp</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>nsp</strong> : Espace de noms à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>bool <strong>DefineVariable</strong>(<strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Définie une variable</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom dde la variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>bool <strong>DefineVariable</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Définie une variable</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>Count</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre de variables contenues dans l'espace de noms</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> char * <strong>GetKey</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom d'une variable de l'espace de noms</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de la variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetVariable</strong>(unsigned int <strong>idx</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne une variable de l'espace de noms</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>idx</strong> : Index de la variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>int <strong>IndexOf</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'index d'une variable de l'espace de noms</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"</ul>\n" +
	"<p>Retourne -1 si la variable n'existe pas dans cet espace de noms</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>operator[]</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["les-variables"] = "" + 
	"<h1 id=\"les-variables\">Les variables</h1>\n" +
	"<p>Cette page décrit l'ensemble des informations relatives aux variables dans un script.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"déclarer-une-variable\">Déclarer une variable</h2>\n" +
	"<p>Pour déclarer une variable la méthode est la suivante :</p>\n" +
	"<pre><code class=\"litescript\">// Déclaration d'une variable sans assignation\n" +
	"var maVariable;\n" +
	"// Déclaration d'une variable avec assignation\n" +
	"var autreVariable = 5.2;\n" +
	"// Déclaration de plusieurs variables sans assignation\n" +
	"var premiereVariable, secondeVariable;\n" +
	"// Déclaration de plusieurs variables avec et sans assignation\n" +
	"var variableAvec = 5.2, variableSans;</code></pre>\n" +
	"<p>Il faut le mot-clé <strong>var</strong> suivi du nom de la variable. Un nom de variable peut être composé de lettres latines en majuscules et minuscules, de chiffres et des caractères spéciaux <strong>$</strong> et <strong>_</strong>. Il est possible d'en déclarer plusieurs en les séparant d'une virgule et enfin vous pourvez les assigner dès leur déclaration avec le caractère <strong>=</strong> suivi d'une expression. Comme toute instruction, il est obligatoire de terminer une déclaration avec un point-virgule.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"les-types-de-variables\">Les types de variables</h2>\n" +
	"<p>En LiteScript, les variables sont faiblement typées, on dit aussi que c'est un langage typé dynamiquement, c'est à dire qu'il n'est pas nécessaire de préciser le contenu d'une variable lors de sa déclaration et que son type n'est pas fixé (à part dans certains cas).</p>\n" +
	"<p>Ainsi les types existants sont les suivants :</p>\n" +
	"<ul>\n" +
	"<li><strong>Non-défini</strong> : Ce type ne peut être obtenu par assignation et signifie soit qu'une variable n'existe pas, soit qu'un argument, le mot-clé <strong>this</strong> ou la valeur de retour n'a pas été assigné.</li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">if (a == undefined)\n" +
	"    // Cette variable n'existe pas</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Nul</strong> : Ne contient pas de valeur, valeur par défaut lorsque une variable a été déclarée mais pas asssignée</li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">// Est de type nul\n" +
	"var a;\n" +
	"// Est toujours de type nul\n" +
	"a = null;</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Booléen</strong></li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">// a contient vrai\n" +
	"var a = true,\n" +
	"// b contient faut\n" +
	"    b = false;</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Nombre</strong> : entier ou flottant signé</li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">var a = -6,\n" +
	"    b = .8e-3;</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Chaine de caractères</strong> : Accepte les caractères au format UTF-8</li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">var anglais = \"Hello!\",\n" +
	"    francais = \"Bonjour!\",\n" +
	"    espagnol = \"¡Buenos días!\",\n" +
	"    japonais = \"こんにちは (Kon'nichiwa)\";</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Fonction</strong></li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">function f1() {\n" +
	"    return 1;\n" +
	"}\n" +
	"\n" +
	"// OU\n" +
	"\n" +
	"var f2 = function() {\n" +
	"    return 2;\n" +
	"}</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Tableau associatif</strong></li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">var a = [ 0, 1, 2, 3, 4 ],\n" +
	"    b = {\n" +
	"        prenom: \"Percy\",\n" +
	"        nom: \"Jackson\",\n" +
	"        age: 17\n" +
	"    };</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Classe</strong></li>\n" +
	"</ul>\n" +
	"<pre><code>class MaClass {\n" +
	"    var attribut;\n" +
	"    function MaClass(a) {\n" +
	"        this.attribut = a;\n" +
	"    }\n" +
	"}\n" +
	"\n" +
	"// ou\n" +
	"\n" +
	"var MaClass = class {\n" +
	"    var attribut;\n" +
	"    function MaClass(a) {\n" +
	"        this.attribut = a;\n" +
	"    }\n" +
	"};</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Instance de classe</strong></li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">var a = new MaClass(),\n" +
	"    b = new MaClass(\"bonjour\");</code></pre>\n" +
	"<ul>\n" +
	"<li><strong>Espace de noms</strong></li>\n" +
	"</ul>\n" +
	"<pre><code class=\"litescript\">// Entre dans l'espace de noms \"Math\"\n" +
	"namespace Math;</code></pre>";

content["1.0"]["fr"]["litescript__assembly"] = "" + 
	"<h1 id=\"litescript__assembly\">LiteScript::Assembly</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/assembly.hpp\">assembly.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/assembly.cpp\">assembly.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Cette classe permet de faciliter la lecture et l'exécution de scripts sous forme de script assembleur.</p>\n" +
	"<p><strong>Enumérateur</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>ErrorType</strong> :\n" +
	"Type d'erreur rencontré lors de l'analyse syntaxique</p>\n" +
	"<ul>\n" +
	"<li><strong>ASSM_ERROR_NO</strong> : Pas d'erreur</li>\n" +
	"<li><strong>ASSM_ERROR_UNKNOW_CMD</strong> : Commande assembleur inconnue</li>\n" +
	"<li><strong>ASSM_ERROR_EXPECTED_BOOLEAN</strong> : Booléen attendu en paramètre</li>\n" +
	"<li><strong>ASSM_ERROR_EXPECTED_INTEGER</strong> : Entier attendu en paramètre</li>\n" +
	"<li><strong>ASSM_ERROR_EXPECTED_NUMBER</strong> : Nombre/flottant attendu en paramètre</li>\n" +
	"<li><strong>ASSM_ERROR_EXPECTED_STRING</strong> : Chaine de caractères attendu en paramètre</li>\n" +
	"<li><strong>ASSM_ERROR_NUMBER</strong> : Nombre d'erreurs possibles</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publiques</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::array&lt;<strong>const</strong> char *, <a href=\"#litescript__assembly\">ErrorType</a>::<strong>ASSM_ERROR_NUMBER</strong>&gt; <strong>ErrorMsg</strong> :\n" +
	"Liste des messages d'erreur</p>\n" +
	"<p><a href=\"#litescript__assembly\">ErrorType</a> <strong>error</strong> :\n" +
	"Type de la dernière erreur rencontrée</p>\n" +
	"<p>unsigned int <strong>line_error</strong> :\n" +
	"Numéro de line de la dernière erreur rencontrée</p>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"<p><a href=\"#litescript__state\">State</a>&amp; <strong>state</strong> :\n" +
	"Etat du script</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Assembly</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Assembly</strong>(<strong>const</strong> <a href=\"#litescript__assembly\">Assembly</a>&amp; <strong>ass</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>ass</strong> : Assembly à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>Execute</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute un code de script assembleur et retourne la variable au sommet de la pile d'état</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code assembleur à exécuter</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> Ajoute une nouvelle liste d'instructions à l'état du script</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ExecuteAfter</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute un code de script assembleur et retourne la variable au sommet de la pile d'état</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code assembleur à exécuter</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> Ajoute les instructions à la liste courante de l'état du script</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ExecuteSingle</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute une seule instruction assembleur et retourne la variable au sommet de la pile d'état</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Instruction assembleur à exécuter</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> Ajoute l'instruction à la liste courante de l'état du script</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> char * <strong>GetError</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le message correspondant à la dernière erreur rencontrée</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <a href=\"#la-structure-instruction\">Instruction</a> <strong>GetInstructionSingle</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant à une instruction assembleur fournie</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script assembleur</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <a href=\"#la-structure-instruction\">Instruction</a> <strong>GetInstructionSingle</strong>(<strong>const</strong> char * <strong>code</strong>, <a href=\"#litescript__assembly\">ErrorType</a>&amp; <strong>err</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant à une instruction assembleur fournie</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script assembleur</li>\n" +
	"<li><strong>err</strong> : Retourne l'erreur rencontrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>GetInstructionList</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant au code assembleur fourni</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script assembleur</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>GetInstructionList</strong>(<strong>const</strong> char * <strong>code</strong>, <a href=\"#litescript__assembly\">ErrorType</a>&amp; <strong>err</strong>, unsigned int&amp; line_err)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant au code assembleur fourni</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script assembleur</li>\n" +
	"<li><strong>err</strong> : Retourne l'erreur rencontrée</li>\n" +
	"<li><strong>line_err</strong> : Numéro de ligne où l'erreur est survenue</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <strong>const</strong> char * <strong>GetError</strong>(<a href=\"#litescript__assembly\">ErrorType</a> <strong>type</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le message d'erreur correspondant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>type</strong> : Type d'erreur</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__instruction"] = "" + 
	"<h1 id=\"litescript__instruction\">LiteScript::Instruction</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/instruction.hpp\">instruction.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/instruction.cpp\">instruction.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Une instruction permet de transcrire un script syntaxiquement complexe à exécuter en une suite d'instructions élémentaires. Elle contient un numéro qui définie le code à exécuter ainsi qu'un argument (<strong>vide</strong>, <strong>booléen</strong>, <strong>entier</strong>, <strong>nombre</strong> ou <strong>chaine de caractères</strong>). Elle est définie comme une structure et non comme une classe.</p>\n" +
	"<h2 id=\"la-structure-instruction\">La structure Instruction</h2>\n" +
	"<p><strong>Enumérateur</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>CompType</strong> :\n" +
	"Type d'argument d'une instruction</p>\n" +
	"<ul>\n" +
	"<li><strong>COMP_TYPE_NONE</strong> : Pas d'argument</li>\n" +
	"<li><strong>COMP_TYPE_BOOLEAN</strong> : Booléen (<em>bool</em>)</li>\n" +
	"<li><strong>COMP_TYPE_INTEGER</strong> : Entier (<em>int</em>)</li>\n" +
	"<li><strong>COMP_TYPE_FLOAT</strong> : Flottant (<em>float</em>)</li>\n" +
	"<li><strong>COMP_TYPE_STRING</strong> : Chaine de caractères (<em>char *</em>)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs</strong></p>\n" +
	"<blockquote>\n" +
	"<p>unsigned char <strong>code</strong> :\n" +
	"Numéro d'instruction élémentaire</p>\n" +
	"<p>unsigned char <strong>comp_type</strong> :\n" +
	"Type d'argument</p>\n" +
	"<p>union <strong>comp_value</strong> :\n" +
	"Valeur de l'argument</p>\n" +
	"<ul>\n" +
	"<li>bool <strong>v_boolean</strong></li>\n" +
	"<li>int <strong>v_integer</strong></li>\n" +
	"<li>float <strong>v_float</strong></li>\n" +
	"<li>char * <strong>v_string</strong></li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeur privé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>Instruction</strong>()</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs/Destructeur publiques</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<a href=\"#les-instructions-élémentaires\">InstrCode</a> <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur d'instruction sans argument</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code de l'instruction</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<a href=\"#les-instructions-élémentaires\">InstrCode</a> <strong>code</strong>, bool <strong>val</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur d'instruction avec un argument booléen</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code de l'instruction</li>\n" +
	"<li><strong>val</strong> : Valeur de l'argument</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<a href=\"#les-instructions-élémentaires\">InstrCode</a> <strong>code</strong>, int <strong>val</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur d'instruction avec un argument entier</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code de l'instruction</li>\n" +
	"<li><strong>val</strong> : Valeur de l'argument</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<a href=\"#les-instructions-élémentaires\">InstrCode</a> <strong>code</strong>, float <strong>val</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur d'instruction avec un argument flottant</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code de l'instruction</li>\n" +
	"<li><strong>val</strong> : Valeur de l'argument</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<a href=\"#les-instructions-élémentaires\">InstrCode</a> <strong>code</strong>, <strong>const</strong> char * <strong>val</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur d'instruction avec un argument chaine de caractères</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code de l'instruction</li>\n" +
	"<li><strong>val</strong> : Valeur de l'argument</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Instruction</strong>(<strong>const</strong> <a href=\"#la-structure-instruction\">Instruction</a>&amp; <strong>i</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Constructeur par copie</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Instruction à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>~Instruction</strong>()</li>\n" +
	"</ul>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>Save</strong>(std::ostream&amp; <strong>stream</strong>, <strong>const</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt;&amp; <strong>instr</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde une liste d'instructions au format textuel dans un flux de données</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"<li><strong>instr</strong> : Liste d'instructions</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>Load</strong>(std::istream&amp; <strong>stream</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge une liste d'instructions d'un flux de données textuelles et la retourne</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>SaveBinary</strong>(std::ostream&amp; <strong>stream</strong>, <strong>const</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt;&amp; <strong>instr</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde une liste d'instructions au format binaire dans un flux de données\n" +
	"(les données sont enregistrées en little endian)</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"<li><strong>instr</strong> : Liste d'instructions</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>LoadBinary</strong>(std::istream&amp; <strong>stream</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge une liste d'instructions d'un flux de données binaires et la retourne\n" +
	"(les données sont chargées en little endian)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateur surchargé</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-structure-instruction\">Instruction</a>&amp; <strong>operator=</strong>(<strong>const</strong> <a href=\"#la-structure-instruction\">Instruction</a>&amp; <strong>in</strong>)</p>\n" +
	"<p>Réassigne l'instruction courante</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>in</strong> : Instruction à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<h2 id=\"les-instructions-élémentaires\">Les instructions élémentaires</h2>\n" +
	"<p>Certaines instructions nécessitent un argument ou qu'un certain nombre de variables soient présentes dans la pile d'état du script.</p>\n" +
	"<p>Le nombre total d'instructions est défini par <strong>INSTR_NUMBER</strong>.</p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_INVALID</strong> :\n" +
	"Instruction invalide</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives à la définition de variables</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_DEFINE_VARIABLE</strong> :\n" +
	"Définie une variable dans l'espace de noms courant</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Nécessite le nom de la variable à définir en paramètre</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_DEFINE_ARG</strong> :\n" +
	"Définie un argument</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Nécessite l'index de l'argument à définir en paramètre</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_DEFINE_RETURN</strong> :\n" +
	"Définie la valeur de retour</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives à la pile d'état du script</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_VALUE_POP</strong> :\n" +
	"Retire une variable de la pile</p>\n" +
	"<p><strong>INSTR_VALUE_COPY</strong> :\n" +
	"Duplique une variable de la pile</p>\n" +
	"<p><strong>INSTR_VALUE_ASSIGN</strong> :\n" +
	"Ré-insère une nouvelle variable de la pile après utilisation de l'opérateur d'assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_UNDEFINED</strong> :\n" +
	"Insère une variable de type <strong>UNDEFINED</strong> dans la pile</p>\n" +
	"<p><strong>INSTR_VALUE_NULL</strong> :\n" +
	"Insère une variable de type <strong>NIL</strong> dans la pile</p>\n" +
	"<p><strong>INSTR_VALUE_BOOLEAN</strong> :\n" +
	"Insère une variable de type <strong>BOOLEAN</strong> dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite la valeur booléenne de la variable à insérer</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_NUMBER</strong> :\n" +
	"Insère une variable de type <strong>NUMBER</strong> dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite la valeur entière/flottante de la variable à insérer</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_STRING</strong> :\n" +
	"Insère une variable de type <strong>STRING</strong> dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite la valeur chaine de caractères de la variable à insérer</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_CALLBACK</strong> :\n" +
	"Insère une variable de type <strong>CALLBACK</strong> dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le numéro de ligne de l'appel</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_ARRAY</strong>\n" +
	"Insère une variable de type <strong>ARRAY</strong> dans la pile</p>\n" +
	"<p><strong>INSTR_VALUE_CLASS</strong>\n" +
	"Insère une variable de type <strong>CLASS</strong> dans la pile</p>\n" +
	"<p><strong>INSTR_VALUE_OBJECT</strong>\n" +
	"Insère une variable de type <strong>CLASS_OBJECT</strong> dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de type <strong>CLASS</strong> de la pile</li>\n" +
	"<li>Déplace une liste d'arguments temporaire vers les arguments effectifs</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_ARG</strong> :\n" +
	"Insère une variable contenu dans la liste effective d'arguments dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite l'index de l'argument en paramètre</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_ARGS</strong> :\n" +
	"Insère une variable de type <strong>ARRAY</strong> contenant des arguments de la liste effective dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite l'index du premier argument à insérer en paramètre</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_VALUE_THIS</strong> :\n" +
	"Insère la variable contenu dans le <strong>THIS</strong> du script dans la pile</p>\n" +
	"<p><strong>INSTR_VALUE_VARIABLE</strong> :\n" +
	"Insère une variable contenue dans l'espace de noms courant dans la pile</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom de la variable en paramètre</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives au espaces de variables et aux appels</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_PUSH_NSP</strong> :\n" +
	"Duplique un nouvel espace de noms dans la liste</p>\n" +
	"<p><strong>INSTR_PUSH_ARGS</strong> :\n" +
	"Crée un nouvel espace d'arguments temporaire dans la liste</p>\n" +
	"<p><strong>INSTR_POP_NSP</strong> :\n" +
	"Retire un espace de noms de la liste</p>\n" +
	"<p><strong>INSTR_POP_ARGS</strong> :\n" +
	"Retire un espace d'arguments temporaire de la liste</p>\n" +
	"<p><strong>INSTR_RETURN</strong> :\n" +
	"Retourne au dernier appel effectué</p>\n" +
	"<ul>\n" +
	"<li>Supprime un espace d'argument effectif</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives aux opérations sur les variables</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_OP_ASSIGN</strong> :\n" +
	"Applique l'opération d'assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_UNARY_PLUS</strong> :\n" +
	"Applique la copie</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_UNARY_MINUS</strong> :\n" +
	"Applique l'opposition</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_PRE_INCR</strong> :\n" +
	"Applique l'opération de pré-incrémentation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_POST_INCR</strong> :\n" +
	"Applique l'opération de post-incrémentation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_PRE_DECR</strong> :\n" +
	"Applique l'opération de pré-décrémentation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_POST_DECR</strong> :\n" +
	"Applique l'opération de post-décrémentation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_ADD</strong> :\n" +
	"Applique l'opération d'addition</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_SUB</strong> :\n" +
	"Applique l'opération de soustraction</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_MUL</strong> :\n" +
	"Applique l'opération de multiplication</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_DIV</strong> :\n" +
	"Applique l'opération de division</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_MOD</strong> :\n" +
	"Applique l'opération modulo</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_EQU</strong> :\n" +
	"Applique l'opération d'égalité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_DIF</strong> :\n" +
	"Applique l'opération d'inégalité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_GREAT</strong> :\n" +
	"Applique l'opération de supériorité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LESS</strong> :\n" +
	"Applique l'opération d'infériorité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_GREAT_EQU</strong> :\n" +
	"Applique l'opération de supériorité ou égalité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LESS_EQU</strong> :\n" +
	"Applique l'opération d'infériorité ou égalité</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LOG_NOT</strong> :\n" +
	"Applique l'opération de négation</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LOG_AND</strong> :\n" +
	"Applique l'opération d'intersection</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LOG_OR</strong> :\n" +
	"Applique l'opération d'union</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_BIT_NOT</strong> :\n" +
	"Applique l'opération de négation binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_BIT_AND</strong> :\n" +
	"Applique l'opération d'intersection binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_BIT_OR</strong> :\n" +
	"Applique l'opération d'union binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_BIT_XOR</strong> :\n" +
	"Applique l'opération d'union exclusif binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_LSHIFT</strong> :\n" +
	"Applique l'opération de décalage à gauche binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_RSHIFT</strong> :\n" +
	"Applique l'opération de décalage à droite binaire</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_ADD_ASSIGN</strong> :\n" +
	"Applique l'opération d'addition/assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_SUB_ASSIGN</strong> :\n" +
	"Applique l'opération de soustraction/assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_MUL_ASSIGN</strong> :\n" +
	"Applique l'opération de multiplication/assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_DIV_ASSIGN</strong> :\n" +
	"Applique l'opération de division/assignation</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_ARRAY</strong> :\n" +
	"Applique l'opération d'accès</p>\n" +
	"<ul>\n" +
	"<li>Retire deux variables de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_MEMBER</strong> :\n" +
	"Récupère le membre d'une variable</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom du membre en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_OP_CALL</strong> :\n" +
	"Applique un appel</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"<li>Ajoute le résultat dans la pile</li>\n" +
	"<li>Déplace une liste d'arguments temporaire vers les arguments effectifs</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives aux sauts d'instructions</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_JUMP_TO</strong> :\n" +
	"Applique un saut d'instructions</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le numéro de l'instruction vers laquelle faire le saut en paramètre</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_JUMP_IF</strong> :\n" +
	"Applique un saut d'instructions si la variable au sommet de la pile est vraie</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le numéro de l'instruction vers laquelle faire le saut en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_JUMP_ELSE</strong> :\n" +
	"Applique un saut d'instructions si la variable au sommet de la pile est fausse</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le numéro de l'instruction vers laquelle faire le saut en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instructions relatives aux variables complexes</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_ARRAY_PUSH_NUMERIC</strong> :\n" +
	"Ajoute une variable à la variable de type <strong>ARRAY</strong> au sommet - 1 de la pile avec un index numérique</p>\n" +
	"<ul>\n" +
	"<li>Nécessite l'index de la variable dans laquelle insérer la variable en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_ARRAY_PUSH_LITERAL</strong> :\n" +
	"Ajoute une variable à la variable de type <strong>ARRAY</strong> au sommet - 1 de la pile avec un index littéral</p>\n" +
	"<ul>\n" +
	"<li>Nécessite l'index sous forme de chaine de caractères de la variable dans laquelle insérer la variable en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_CLASS_PUSH_STATIC</strong> :\n" +
	"Ajoute une variable à la variable de type <strong>CLASS</strong> au sommet - 1 de la pile en tant que membre statique</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom du membre statique en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_CLASS_PUSH_USTATIC</strong> :\n" +
	"Ajoute une variable à la variable de type <strong>CLASS</strong> au sommet - 1 de la pile en tant que membre non statique</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom du membre non statique en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_CLASS_PUSH_OPERATOR</strong> :\n" +
	"Ajoute une variable à la variable de type <strong>CLASS</strong> au sommet - 1 de la pile en tant qu'opérateur surchargé</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le numéro de l'opérateur (<a href=\"#le-type-class\">OperatorType</a>) en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_CLASS_INHERIT</strong>\n" +
	"Ajoute une variable à la variable de type <strong>CLASS</strong> au sommet - 1 de la pile en tant que classe héritée</p>\n" +
	"<ul>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"<p><strong>INSTR_CLASS_CONSTRUCTOR</strong> :\n" +
	"Définie un membre non statique de la variable de type <strong>CLASS</strong> au sommet de la pile en tant que constructeur</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom du membre non statique en paramètre</li>\n" +
	"<li>Retire une variable de la pile</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Instruction relative aux espaces de noms</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>INSTR_NAMESPACE_USE</strong> :\n" +
	"Définie l'espace de noms global à utiliser</p>\n" +
	"<ul>\n" +
	"<li>Nécessite le nom de l'espace de noms par rapport à l'espace de noms principal en paramètre</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__memory"] = "" + 
	"<h1 id=\"litescript__memory\">LiteScript::Memory</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/memory.hpp\">memory.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/memory.cpp\">memory.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Contient tous les objets référencés par des variables. Les variables sont détruites dynamiquement lorsque d'il n'y a plus de variable pour les référencer et un algorithme ramasse-miettes de type <strong><em>mark and sweep</em></strong> permet de supprimer celles qui seraient hors d'atteinte.</p>\n" +
	"<p><strong>Valeurs définies</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>LITESCRIPT_MEMORY_SIZE</strong> :\n" +
	"Nombre de sous-mémoires contenu dans la mémoire principale</p>\n" +
	"<p><strong>LITESCRIPT_MEMORY_MAXCOUNT</strong> :\n" +
	"Nombre maximum de variable pouvant être contenue dans une mémoire</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::array&lt;void *, <strong>LITESCRIPT_MEMORY_SIZE</strong>&gt; <strong>arr</strong> :\n" +
	"Tableau des sous-mémoires</p>\n" +
	"<p>std::array&lt;short, <strong>LITESCRIPT_MEMORY_SIZE</strong>&gt; <strong>nfull</strong> :\n" +
	"Indexeur des sous-mémoires non pleines</p>\n" +
	"<p>short <strong>first_nfull</strong> :\n" +
	"Index de la première sous-mémoire non pleine</p>\n" +
	"<p>unsigned int <strong>count</strong> :\n" +
	"Nombre de variables contenues dans la mémoire</p>\n" +
	"<p>std::vector&lt;<a href=\"#litescript__type\">Type</a> *&gt; <strong>type_list</strong> :\n" +
	"Liste temporaire de types pour la lecture de fichier</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attribut publique</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>Count</strong> :\n" +
	"Nombre de variables contenues dans la mémoire</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeur/destructeur</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>Memory</strong>()</p>\n" +
	"<p><strong>~Memory</strong>()</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>bool <strong>isFull</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Indique si la mémoire est pleine</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>Create</strong>(<a href=\"#litescript__type\">Type</a>&amp; <strong>type</strong> = <a href=\"#litescript__type\">Type</a>::<strong>NIL</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Crée un nouvel objet et retourne sa variable</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>type</strong> (<em>facultatif</em>) : Type de l'objet (<strong>NIL</strong> par défaut)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>Remove</strong>(unsigned int <strong>id</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime un objet en mémoire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>id</strong> : Identifiant de l'objet</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>GetVariable</strong>(unsigned int <strong>id</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère une variable référençant un objet existant (retourne une valeur nulle s'il n'existe pas)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>id</strong> : Identifiant de l'objet en mémoire</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>GarbageCollector</strong>(<strong>const</strong> <a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute l'algorithme ramasse-miettes</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> il est conseillé de ne l'utiliser qu'après une exécution complète du script pour éviter tout problème</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>GarbageCollector</strong>(int <strong>scout</strong>, <strong>const</strong> <a href=\"#litescript__state\">State</a> ** <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute l'algorithme ramasse-miettes</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>scount</strong> : Nombre d'états de script</li>\n" +
	"<li><strong>sarray</strong> : Etats de script</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> il est conseillé de ne l'utiliser qu'après une exécution complète du script pour éviter tout problème</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>Save</strong>(std::ostream&amp; <strong>stream</strong>, <strong>const</strong> <a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ecrit une mémoire dans un flux de données binaire</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en sortie</li>\n" +
	"<li><strong>memory</strong> : Mémoire à sauvegarder</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <a href=\"#litescript__memory\">Memory</a> <strong>Load</strong>(std::istream&amp; <strong>stream</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Lit et retourne une mémoire dans un flux de données binaire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données en entrée</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes privées</strong></p>\n" +
	"<ul>\n" +
	"<li>bool <strong>ProtectVariable</strong>(unsigned int <strong>i</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Protège un objet de l'algorithme ramasse-miettes</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Identifiant de l'objet</li>\n" +
	"</ul>\n" +
	"<p>Retourne vrai si l'objet est déjà protégé</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>bool <strong>SaveVariable</strong>(std::ostream&amp; <strong>stream</strong>, unsigned int <strong>i</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde une variable si ça n'a pas déjà été fait</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Le flux de données dans lequel sauvegarder l'objet</li>\n" +
	"<li><strong>i</strong> : Identifiant de l'objet</li>\n" +
	"</ul>\n" +
	"<p>Rerourne vrai si l'objet a déjà été enregistré\n" +
	"(<strong><em>Remarque</em></strong> : même si l'objet a déjà été enregistré, on écrit tout de même l'ID de l'objet dans le flux)</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>LoadVariable</strong>(std::istream&amp; <strong>stream</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge un objet d'un flux de données et l'enregistre en mémoire</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données</li>\n" +
	"</ul>\n" +
	"<p>Retourne l'identifiant de l'objet</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__object"] = "" + 
	"<h1 id=\"litescript__object\">LiteScript::Object</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/object.hpp\">object.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/object.cpp\">object.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Un objet contient les données relative à une variable et est stocké dans une mémoire.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::allocator&lt;char&gt; <strong>allocator</strong> :\n" +
	"Allocateur de chaque objet</p>\n" +
	"<p><a href=\"#litescript__type\">Type</a> * type :\n" +
	"Type de l'objet</p>\n" +
	"<p>unsigned int <strong>size</strong> :\n" +
	"Taille des données en octets</p>\n" +
	"<p>void * <strong>data</strong> :\n" +
	"Pointeur sur les données</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publics</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>const</strong> unsigned int <strong>ID</strong> :\n" +
	"Identifiant de l'objet en mémoire</p>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; memory :\n" +
	"Mémoire contenant l'objet</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs/destructeur</strong></p>\n" +
	"<ul>\n" +
	"<li>Object(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>mem</strong>, unsigned int <strong>id</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>mem</strong> : Mémoire contenant l'objet</li>\n" +
	"<li><strong>id</strong> : Identifiant de l'objet en mémoire</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>Object(<strong>const</strong> <a href=\"#litescript__object\">Object</a>&amp; object)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Objet à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>~Object()</li>\n" +
	"</ul>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>Clear</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Supprime le contenu de l'objet</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__type\">Type</a>&amp; GetType() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le type de l'objet</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong><em>T</em></strong>&amp; <strong>GetData</strong>&lt;<strong><em>T</em></strong>&gt;()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne les données contenues dans l'objet avec le type indiqué dans le template</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> <strong><em>T</em></strong>&amp; <strong>GetData</strong>&lt;<strong><em>T</em></strong>&gt;() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne les données contenues dans l'objet avec le type indiqué dans le template (constant)</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__object\">Object</a>&amp; <strong>Reassign</strong>(<a href=\"#litescript__type\">Type</a>&amp; <strong>type</strong>, unsigned int <strong>size</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Réassigne le contenu de l'objet</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>type</strong> : Nouveau type de l'objet</li>\n" +
	"<li><strong>size</strong> : Nouvelle taille des données</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__script"] = "" + 
	"<h1 id=\"litescript__script\">LiteScript::Script</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/script.hpp\">script.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/script.cpp\">script.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Cette classe permet de faciliter la lecture et l'exécution de scripts.</p>\n" +
	"<p><strong>Enumérateur</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>ErrorType</strong> :\n" +
	"Type d'erreur rencontré lors de l'analyse syntaxique</p>\n" +
	"<ul>\n" +
	"<li><strong>SCRPT_ERROR_NO</strong> : Pas d'erreur rencontrée</li>\n" +
	"<li><strong>SCRPT_ERROR_SIMPLEARRAY_END</strong> : Crochet fermant &quot;]&quot; attendu pour la fermetture d'une structure de tableau simple</li>\n" +
	"<li><strong>SCRPT_ERROR_NAMEDARRAY_COLON</strong> : Deux-points &quot;:&quot; attendu après le nom d'un membre de tableau associatif</li>\n" +
	"<li><strong>SCRPT_ERROR_NAMEDARRAY_EXPRESSION</strong> : Expression attendue après le nom d'un membre de tableau associatif</li>\n" +
	"<li><strong>SCRPT_ERROR_NAMEDARRAY_END</strong> : Accolade fermante &quot;}&quot; attendue pour la fermetture d'une structure de tableau associatif</li>\n" +
	"<li><strong>SCRPT_ERROR_CALLBACK_ARGUMENTS</strong> : Arguments attendus pour la définition d'une fonction</li>\n" +
	"<li><strong>SCRPT_ERROR_CALLBACK_ARGUMENT_NAME</strong> : Nom de variable attendu pour la définition des arguments</li>\n" +
	"<li><strong>SCRPT_ERROR_CALLBACK_ARGUMENT_END</strong> : Parenthèse fermante &quot;)&quot; attendue pour la fermetture des arguments</li>\n" +
	"<li><strong>SCRPT_ERROR_CALLBACK_INSTRUCTIONS</strong> : Instructions attendues pour la définition d'une fonction</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_INHERIT</strong> : Classe à hériter attendue pour la definition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_BEGIN</strong> : Accolade ouvrante &quot;{&quot; attendue pour la définition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_INVALID</strong> : Membre invalide dans la définition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_OPERATOR_NAME</strong> : Nom de l'opérateur attendu pour la définition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_OPERATOR_INVALID</strong> : Nom de l'opérateur inconnu pour la définition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_CLASS_END</strong> : Accolade fermante &quot;}&quot; attendue pour la définition de la classe</li>\n" +
	"<li><strong>SCRPT_ERROR_FOR_INITIALISATION</strong> : Initialisation attendue dans la structure de contrôle &quot;for&quot;</li>\n" +
	"<li><strong>SCRPT_ERROR_FOR_CONDITION</strong> : Condition attendue dans la structure de contrôle &quot;for&quot;</li>\n" +
	"<li><strong>SCRPT_ERROR_SWITCH_KEYWORDS</strong> : Mot-clé &quot;case&quot; ou &quot;default&quot; attendu dans la structure de contrôle &quot;switch&quot;</li>\n" +
	"<li><strong>SCRPT_ERROR_SWITCH_END</strong> : Accolade fermante &quot;}&quot; attendue pour la structure de contrôle &quot;switch&quot;</li>\n" +
	"<li><strong>SCRPT_ERROR_NAME</strong> : Nom de variable attendu</li>\n" +
	"<li><strong>SCRPT_ERROR_VALUE</strong> : Opérande attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_OPERATOR</strong> : Opérateur attendu</li>\n" +
	"<li><strong>SCRPT_ERROR_EXPRESSION</strong> : Expression attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_PARENTHESIS_OPEN</strong> : Parenthèse ouvrante &quot;(&quot; attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_PARENTHESIS_CLOSE</strong> : Parenthèse fermante &quot;)&quot; attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_BRACKET_CLOSE</strong> : Crochet fermant &quot;]&quot; attendu</li>\n" +
	"<li><strong>SCRPT_ERROR_BRACE_OPEN</strong> : Accolade ouvrante &quot;{&quot; attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_BRACE_CLOSE</strong> : Accolade fermante &quot;}&quot; attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_COLON</strong> : Deux-points &quot;:&quot; attendus</li>\n" +
	"<li><strong>SCRPT_ERROR_SEMICOLON</strong> : Point-virgule &quot;;&quot; attendu</li>\n" +
	"<li><strong>SCRPT_ERROR_INSTRUCTION</strong> : Instruction attendue</li>\n" +
	"<li><strong>SCRPT_ERROR_RETURN</strong> : Expression ou point-virgule attendu pour l'instruction de retour</li>\n" +
	"<li><strong>SCRPT_ERROR_WHILE</strong> : Mot-clé &quot;while&quot; attendu pour la structure de contrôle &quot;do..while&quot;</li>\n" +
	"<li><strong>SCRPT_ERROR_NAMESPACE</strong> : Espace de nom invalide</li>\n" +
	"<li><strong>SCRPT_ERROR_UNKNOW</strong> : Element inconnu</li>\n" +
	"<li><strong>SCRPT_ERROR_NUMBER</strong> : Nombre d'erreurs possibles</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publiques</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::array&lt;<strong>const</strong> char *, <a href=\"#litescript__script\">ErrorType</a>::<strong>SCRPT_ERROR_NUMBER</strong>&gt; <strong>ErrorMsg</strong> :\n" +
	"Liste des messages d'erreur</p>\n" +
	"<p><a href=\"#litescript__script\">ErrorType</a> <strong>error</strong> :\n" +
	"Type de la dernière erreur rencontrée</p>\n" +
	"<p>unsigned int <strong>line_error</strong> :\n" +
	"Numéro de line de la dernière erreur rencontrée</p>\n" +
	"<p>unsigned int <strong>col_error</strong> :\n" +
	"Numéro de colonne de la dernière erreur rencontrée</p>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"<p><a href=\"#litescript__state\">State</a>&amp; <strong>state</strong> :\n" +
	"Etat du script</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Script</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Script</strong>(<strong>const</strong> <a href=\"#litescript__script\">Script</a>&amp; sc)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>sc</strong> : Script à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>void <strong>Execute</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute un code de script</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code assembleur à exécuter</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> Ajoute une nouvelle liste d'instructions à l'état du script</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> char * <strong>GetError</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le message correspondant à la dernière erreur rencontrée</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>GetInstructionList</strong>(<strong>const</strong> char * <strong>code</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant au code fourni</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt; <strong>GetInstructionList</strong>(<strong>const</strong> char * <strong>code</strong>, <a href=\"#litescript__script\">ErrorType</a>&amp; <strong>err</strong>, unsigned int&amp; line_err, unsigned int&amp; col_error)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retoune la liste des instructions correspondant au code fourni</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>code</strong> : Code du script</li>\n" +
	"<li><strong>err</strong> : Retourne l'erreur rencontrée</li>\n" +
	"<li><strong>line_err</strong> : Numéro de ligne où l'erreur est survenue</li>\n" +
	"<li><strong>col_err</strong> : Numéro de colonne où l'erreur est survenue</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <strong>const</strong> char * <strong>GetError</strong>(<a href=\"#litescript__script\">ErrorType</a> <strong>type</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le message d'erreur correspondant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>type</strong> : Type d'erreur</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__state"] = "" + 
	"<h1 id=\"litescript__state\">LiteScript::State</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/state.hpp\">state.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/script\">script</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/script/state.cpp\">state.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>L'état de script permet de gérer toute l'exécution du script en stockant les listes d'instructions, la mémoire, les espaces de noms, la position dans la liste d'instruction, etc...</p>\n" +
	"<p><strong>Classe amie</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#la-classe-stateexecutor\">StateExecutor</a></p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>std::vector&lt;std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt;&gt; <strong>instr</strong> :\n" +
	"Listes d'instructions à exécuter</p>\n" +
	"<p>unsigned int <strong>instr_index</strong> :\n" +
	"Index de la liste d'instructions courante</p>\n" +
	"<p>unsigned int <strong>line_num</strong> :\n" +
	"Index de l'instruction courante</p>\n" +
	"<p><a href=\"#litescript__variable\">Variable</a> <strong>nsp_global</strong> :\n" +
	"Espace de noms principal contenu dans une variable</p>\n" +
	"<p>std::vector&lt;std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>args_tmp</strong> :\n" +
	"Liste des espaces d'arguments temporaires</p>\n" +
	"<p>std::vector&lt;std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>args_def</strong> :\n" +
	"Liste des espaces d'arguments effectifs</p>\n" +
	"<p>std::vector&lt;<a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>ths</strong> :\n" +
	"Liste des variables <strong>this</strong></p>\n" +
	"<p>std::vector&lt;<a href=\"#la-classe-nullable\">Nullable</a>&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&gt; <strong>rets</strong> :\n" +
	"Liste des variables de retour</p>\n" +
	"<p>std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt; <strong>op_lifo</strong> :\n" +
	"Pile d'état du script</p>\n" +
	"<p>std::vector&lt;std::array&lt;unsigned int, 2&gt;&gt; <strong>call_lifo</strong> :\n" +
	"Pile des appels</p>\n" +
	"<p>std::vector&lt;<a href=\"#la-classe-namer\">Namer</a>&gt; <strong>nsp_lifo</strong> :\n" +
	"Liste des espaces de noms</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs publiques</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong> :\n" +
	"Mémoire principale</p>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>InstructionIndex</strong> :\n" +
	"Index de la liste d'instructions courante</p>\n" +
	"<p><strong>const</strong> unsigned int&amp; <strong>InstructionLine</strong> :\n" +
	"Index de l'instruction courante</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>State</strong>(<a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>State</strong>(<strong>const</strong> <a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : Etat de script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>Execute</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute le reste des instructions</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ExecuteSingle</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute une instruction</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>ExecuteSingle</strong>(<strong>const</strong> <a href=\"#la-structure-instruction\">Instruction</a>&amp; <strong>instr</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Exécute une instruction qui ne se trouve pas la liste</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>instr</strong> : Instruction à exécuter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>bool <strong>Finished</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Indique si l'exécution du script est terminée</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddInstruction</strong>(<strong>const</strong> <a href=\"#la-structure-instruction\">Instruction</a>&amp; <strong>in</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute une instruction à la liste courante</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>in</strong> : Instruction à ajouter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddInstructions</strong>(<strong>const</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt;&amp; <strong>in_list</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute une liste d'instructions</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>in_list</strong> : Liste d'instructions à ajouter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> std::vector&lt;<a href=\"#la-structure-instruction\">Instruction</a>&gt;&amp; <strong>GetInstruction</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère une liste d'instruction</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index de la liste d'instructions</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetVariable</strong>(<strong>const</strong> char * <strong>name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère une variable dans l'espace de noms courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>DeclareVariable</strong>(<strong>const</strong> char * <strong>name</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Déclare une variable dans l'espace de noms courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom de la variable</li>\n" +
	"<li><strong>v</strong> : Variable à ajouter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#la-classe-namer\">Namer</a>&amp; <strong>GetNamer</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère l'espace de noms courant</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>UseNamespace</strong>(<strong>const</strong> char * <strong>name</strong> = <em>&quot;global&quot;</em>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Change d'espace de noms global</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> (<em>facultatif</em>) : Nom de l'espace de noms dans l'espace de noms principal</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>DefineArgs</strong>(<strong>const</strong> std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute la liste aux arguments effectifs</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>args</strong> : Liste d'arguments à ajouter</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetArgsCount</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nombre d'arguments dans l'espace courant</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetArg</strong>(unsigned int <strong>i</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère un argument dans l'espace courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>i</strong> : Index de l'argument</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetThis</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Récupère la variable <strong>this</strong> de l'espace courant</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>SetThis</strong>(<strong>const</strong> <a href=\"#litescipt__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Assigne la variable <strong>this</strong> de l'espace courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>SetReturn</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Assigne la variable de retour de l'espace courant</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Variable à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>AddCallback</strong>(unsigned int <strong>index</strong>, unsigned int <strong>line</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Ajoute un retour d'appel</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>index</strong> : Index de la liste des instructions</li>\n" +
	"<li><strong>line</strong> : Index de l'instruction</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>RemoveCallback</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique et supprime un retour d'appel</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>GarbageCollector</strong>(bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(unsigned int)) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Protège les variables accessibles dans l'état du script de l'algorithme ramasse-miettes</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>caller</strong> : Fonction prenant en paramètre l'identifiant de l'objet à protéger (retourne vrai si déjà protégé)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> void <strong>Save</strong>(std::ostream&amp; <strong>stream</strong>, <strong>const</strong> <a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde l'état d'un script dans un flux de données binaire</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données sortant</li>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <a href=\"#litescript__state\">State</a> <strong>Load</strong>(std::istream&amp; <strong>stream</strong>, <a href=\"#litescript__memory\">Memory</a>&amp; <strong>memory</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge l'état d'un script dans un flux de données binaire et le retourne</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Flux de données entrant</li>\n" +
	"<li><strong>memory</strong> : Mémoire principale</li>\n" +
	"</ul>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__type"] = "" + 
	"<h1 id=\"litescript__type\">LiteScript::Type</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/type.hpp\">type.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/type.cpp\">type.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Cette classe est une abstraction de tous les types de variables (<code>undefined</code>, <code>null</code>, <code>boolean</code>, <code>number</code>, etc...) qui héritent de celle-ci. Elle permet de contrôler toutes les opérations correspondantes au type de la variable. Tous types nouvellement créés devront hériter de celle-ci pour pouvoir être utilisés.</p>\n" +
	"<p><strong>Attributs publics</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>UNDEFINED</strong> :\n" +
	"Type &quot;non-défini&quot; (pas de données supplémentaires)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>NIL</strong> :\n" +
	"Type &quot;nul&quot; (pas de données supplémentaires)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>BOOLEAN</strong> :\n" +
	"Type &quot;booléen&quot; (les données sont de type <em>bool</em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>NUMBER</strong> :\n" +
	"Type &quot;nombre&quot; (les données sont de type <em><a href=\"#la-classe-number\">Number</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>STRING</strong> :\n" +
	"Type &quot;chaine de caractères&quot; (les données sont de type <em><a href=\"#la-classe-string\">String</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>CALLBACK</strong> :\n" +
	"Type &quot;appel de fonction&quot; (les données sont de type <em><a href=\"#la-classe-callback\">Callback</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>ARRAY</strong> :\n" +
	"Type &quot;tableau (associatif)&quot; (les données sont de type <em><a href=\"#la-classe-array\">Array</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>CLASS</strong> :\n" +
	"Type &quot;classe&quot; (les données sont de type <em><a href=\"#la-classe-class\">Class</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>CLASS_OBJECT</strong> :\n" +
	"Type &quot;élément de classe&quot; (les données sont de type <em><a href=\"#la-classe-classobject\">ClassObject</a></em>)</p>\n" +
	"<p><strong>static</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>NAMESPACE</strong> :\n" +
	"Type &quot;espace de noms&quot; (les données sont de type <em><a href=\"#la-classe-namespace\">Namespace</a></em>)</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p>unsigned int <strong>id</strong> :\n" +
	"Identifiant du type</p>\n" +
	"<p>std::string <strong>name</strong> :\n" +
	"Nom du type</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeur</strong></p>\n" +
	"<blockquote>\n" +
	"<p>Type(<strong>const</strong> char * <strong>name</strong>)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du type</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li>unsigned int <strong>GetID</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'identifiant du type</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>std::string <strong>GetName</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le nom du type</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>static</strong> <strong>const</strong> std::vector&lt;<a href=\"#litescript__type\">Type</a> *&gt;&amp; <strong>GetTypesList</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne la liste des types enregistrés</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>Convert</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>, <strong>const</strong> <a href=\"#litescript__type\">Type</a>&amp; type) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Essaie de convertir une variable dans un autre type et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Variable à convertir</li>\n" +
	"<li><strong>type</strong> : Nouveau type</li>\n" +
	"</ul>\n" +
	"<p><strong><em>Remarque :</em></strong> la nouvelle variable n'est pas forcément du type souhaité si la conversion est impossible</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__object\">Object</a>&amp; <strong>AssignObject</strong>(<a href=\"#litescript__object\">Object</a>&amp; <strong>object</strong>) = 0</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Assigne l'objet avec les données correspondant au type puis retourne le même objet (<strong><em>tout nouveau type doit obligatoirement redéfinir cette méthode</em></strong>)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Objet à assigner</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> void <strong>ODestroy</strong>(<a href=\"#litescript__object\">Object</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Détruit les éléments alloués lors de la création de l'objet</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Objet à détruire</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OAssign</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_target</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'assignation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_target</strong> : Première opérande</li>\n" +
	"<li><strong>object_src</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OUnaryPlus</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la copie et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OUnaryMinus</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opposé et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OPreIncrement</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la pré-incrémentation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OPostIncrement</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la post-incrémentation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OPreDecrement</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la pré-décrémentation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OPostDecrement</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la post-décrémentation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OAdd</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'addition et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OSubstract</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de soustraction et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OMultiply</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de multiplication et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>ODivide</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de division et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OModulo</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération modulo et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OEqual</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'égalité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>ONotEqual</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'inégalité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OGreater</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de supériorité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLess</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'infériorité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OGreaterOrEqual</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de supériorité ou égalité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLessOrEqual</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'infériorité ou égalité et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLogicalNot</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de négation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLogicalAnd</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'intersection et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLogicalOr</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'union et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OBitwiseNot</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de négation binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OBitwiseAnd</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'intersection binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OBitwiseOr</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'union binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OBitwiseXor</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'union exclusif binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OLeftShift</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de décalage à gauche binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>ORightShift</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_1</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_2</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de décalage à droite binaire et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_1</strong> : Première opérande</li>\n" +
	"<li><strong>object_2</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OAddAndAssign</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_target</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'addition/assignation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_target</strong> : Première opérande</li>\n" +
	"<li><strong>object_src</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OSubstractAndAssign</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_target</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de soustraction/assignation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_target</strong> : Première opérande</li>\n" +
	"<li><strong>object_src</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OMultiplyAndAssign</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_target</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de multiplication/assignation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_target</strong> : Première opérande</li>\n" +
	"<li><strong>object_src</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>ODivideAndAssign</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_target</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de division/assignation et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_target</strong> : Première opérande</li>\n" +
	"<li><strong>object_src</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OArray</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>, <strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_key</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'accès et retourne le résultat</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_src</strong> : Première opérande</li>\n" +
	"<li><strong>object_key</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OMember</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object_src</strong>, <strong>const</strong> char * <strong>member_name</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un membre contenu dans la variable (peut retourner une variable de type <strong>UNDEFINED</strong> s'il n'existe pas)</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object_src</strong> : Variable</li>\n" +
	"<li><strong>member_name</strong> : Nom du membre</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> <a href=\"#litescript__variable\">Variable</a> <strong>OCall</strong>(<a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>, <a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le résultat d'un appel de fonction</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Variable</li>\n" +
	"<li><strong>state</strong> : Etat du script</li>\n" +
	"<li><strong>args</strong> : Arguments de l'appel</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> std::string <strong>ToString</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie la variable est chaine de caractères</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Variable à convertir</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> void <strong>Save</strong>(std::ostream&amp; <strong>stream</strong>, <a href=\"#litescript__object\">Object</a>&amp; <strong>object</strong>, bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(std::ostream&amp;, unsigned int)) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Sauvegarde le contenu d'un objet dans un flux de données binaires</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Le flux en sortie</li>\n" +
	"<li><strong>object</strong> : L'objet à sauvegarder</li>\n" +
	"<li><strong>caller</strong> : Fonction permettant de sauvegarder une variable de la mémoire dans le flux de données (si déjà enregistrée, sauvegarde uniquement son ID)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> void <strong>Load</strong>(std::istream&amp; <strong>stream</strong>, <a href=\"#litescript__object\">Object</a>&amp; <strong>object</strong>, unsigned int (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(std::istream&amp;)) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Charge le contenu d'un objet d'un flux de données binaires</p>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>stream</strong> : Le flux en entrée</li>\n" +
	"<li><strong>object</strong> : L'objet à charger</li>\n" +
	"<li><strong>caller</strong> : Fonction permettant de charger une variable dans la mémoire si elle n'existe pas encore (retourne son ID)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>virtual</strong> void <strong>GarbageCollector</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>, bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(unsigned int)) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Permet d'exécuter l'algorithme ramasse-miettes pour la mémoire (son utilisation est déconseillée)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Variable à protéger</li>\n" +
	"<li><strong>caller</strong> : Fonction prenant en paramètre l'identifiant de l'objet à protéger (retourne vrai si déjà protégé)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Méthode privée</strong></p>\n" +
	"<blockquote>\n" +
	"<p><strong>static</strong> std::vector&lt;<a href=\"#litescript__type\">Type</a> *&gt;&amp; <strong>_GetTypesList</strong>()</p>\n" +
	"<p>Retourne la liste des types enregistrés</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["litescript__variable"] = "" + 
	"<h1 id=\"litescript__variable\">LiteScript::Variable</h1>\n" +
	"<table>\n" +
	"<thead>\n" +
	"<tr>\n" +
	"<th>Fichiers sources</th>\n" +
	"<th></th>\n" +
	"</tr>\n" +
	"</thead>\n" +
	"<tbody>\n" +
	"<tr>\n" +
	"<td><em>Header</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/variable.hpp\">variable.hpp</a></td>\n" +
	"</tr>\n" +
	"<tr>\n" +
	"<td><em>Source</em></td>\n" +
	"<td><a href=\"https://github.com/LePtitDev/lite-script/tree/master/src/memory\">memory</a>/<a href=\"https://github.com/LePtitDev/lite-script/blob/master/src/memory/variable.cpp\">variable.cpp</a></td>\n" +
	"</tr>\n" +
	"</tbody>\n" +
	"</table>\n" +
	"<hr />\n" +
	"<p>Une variable référence un objet contenu en mémoire, lorsque toutes les variables référençant l'objet sont détruits, l'objet est supprimé de la mémoire.</p>\n" +
	"<p><strong>Attributs privés</strong></p>\n" +
	"<blockquote>\n" +
	"<p><a href=\"#litescript__object\">Object</a>&amp; <strong>obj</strong> : Référence sur l'objet en mémoire</p>\n" +
	"<p>unsigned int&amp; <strong>nb_ref</strong> : Référence sur le compteur de références</p>\n" +
	"</blockquote>\n" +
	"<p><strong>Constructeurs/destructeur</strong></p>\n" +
	"<ul>\n" +
	"<li><strong>Variable</strong>(<a href=\"#litescript__object\">Object</a>&amp; <strong>obj</strong>, unsigned int&amp; <strong>nb_ref</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètres :</p>\n" +
	"<ul>\n" +
	"<li><strong>obj</strong> : Référence sur l'objet en mémoire</li>\n" +
	"<li><strong>nb_ref</strong> : Référence sur le compteur de références</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>Variable</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>v</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>v</strong> : Variable à copier</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>~Variable</strong>()</li>\n" +
	"</ul>\n" +
	"<p><strong>Méthodes publiques</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>Convert</strong>(<strong>const</strong> <a href=\"#litescript__type\">Type</a>&amp; <strong>type</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Essaie de convertir un objet avec un autre type</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>type</strong> : Type de conversion</li>\n" +
	"</ul>\n" +
	"<p>Retourne un nouvel objet</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>GetMember</strong>(<strong>const</strong> char * <strong>name</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne le membre contenu dans la variable</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>name</strong> : Nom du membre contenu dans la variable</li>\n" +
	"</ul>\n" +
	"<p>Peut retourner une variable de type <strong>UNDEFINED</strong> si le membre n'existe pas</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li>void <strong>GarbageCollector</strong>(bool (<a href=\"#litescript__memory\">Memory</a>::*<strong>caller</strong>)(unsigned int))</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Permet d'exécuter l'algorithme ramasse-miettes pour la mémoire (son utilisation est déconseillée)</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>caller</strong> : Fonction prenant en paramètre l'identifiant de l'objet à protéger (retourne vrai si déjà protégé)</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<p><strong>Opérateurs surchargés</strong></p>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__object\">Object</a>* <strong>operator-&gt;</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'objet référencé</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> <a href=\"#litescript__object\">Object</a>* <strong>operator-&gt;</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne l'objet référencé (constant)</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__object\">Object</a>&amp; <strong>operator*</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur sur l'objet référencé</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>const</strong> <a href=\"#litescript__object\">Object</a>&amp; <strong>operator*</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Retourne un pointeur sur l'objet référencé (constant)</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'assignation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator+</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la copie et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator-</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opposé et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator++</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la pré-incrémentation et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator++</strong>(int)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la post-incrémentation et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator--</strong>()</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la pré-décrémentation et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator--</strong>(int)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la post-décrémentation et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator+</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'addition et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator-</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de soustraction et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator*</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de multiplication et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator/</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de division et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator%</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération modulo et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator==</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'égalité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator!=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'inégalité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&gt;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de supériorité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&gt;=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de supériorité ou égalité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&lt;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'infériorité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&lt;=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'infériorité ou égalité et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator!</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de négation et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&amp;&amp;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'intersection et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator||</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'union et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator~</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique la négation binaire et retourne le résultat</p>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&amp;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'intersection binaire et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator|</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'union binaire et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator^</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'union exclusif binaire et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&lt;&lt;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique le décalage à gauche binaire et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator&gt;&gt;</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>) <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique le décalage à droite binaire et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator+=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'addition/assignation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator-=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de soustraction/assignation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator*=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de multiplication/assignation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator/=</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération de division/assignation et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator[]</strong>(<strong>const</strong> <a href=\"#litescript__variable\">Variable</a>&amp; <strong>object</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'accès et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>object</strong> : Seconde opérande</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><a href=\"#litescript__variable\">Variable</a> <strong>operator()</strong>(<a href=\"#litescript__state\">State</a>&amp; <strong>state</strong>, std::vector&lt;<a href=\"#litescript__variable\">Variable</a>&gt;&amp; <strong>args</strong>)</li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Applique l'opération d'appel et retourne le résultat</p>\n" +
	"<p>Paramètre :</p>\n" +
	"<ul>\n" +
	"<li><strong>state</strong> : L'état du script</li>\n" +
	"<li><strong>args</strong> : Les arguments de l'appel</li>\n" +
	"</ul>\n" +
	"</blockquote>\n" +
	"<ul>\n" +
	"<li><strong>operator std::string</strong>() <strong>const</strong></li>\n" +
	"</ul>\n" +
	"<blockquote>\n" +
	"<p>Convertie la variable en chaine de caractères</p>\n" +
	"</blockquote>";

content["1.0"]["fr"]["petit-tour-rapide"] = "" + 
	"<h1 id=\"petit-tour-rapide\">Petit tour rapide</h1>\n" +
	"<p>Il y a tout d'abord plusieurs informations à connaitre qui peuvent permettre à des développeurs habitués d'apréhender le langage de script plus rapidement. Pour avoir plus de détail, lisez les pages suivantes.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"un-moteur-de-script-avant-tout\">Un moteur de script avant tout</h2>\n" +
	"<p>La première des choses à comprendre est que LiteScript est avant tout un moteur de script dont la vocation est d'être embarqué dans des applications. Ce qui a plusieurs conséquences :</p>\n" +
	"<ol>\n" +
	"<li>Pour que le développeur de l'application ait une maitrise totale du langage il n'y a aucune fonction implémenté de base, c'est à lui d'ajouter les fonctions nécessaires et les documenter pour son application. Si vous lisez cette documentation dans le but de programmer des scripts dans une application, pensez à vous renseigner sur les fonctions ajoutés dans celle-ci.</li>\n" +
	"<li>Le moteur est libre de droit donc modifiable ce qui veut dire qu'il est possible que langage soit légèrement différent d'une application à l'autre.</li>\n" +
	"</ol>\n" +
	"<hr />\n" +
	"<h2 id=\"similaire-à-javascript\">Similaire à Javascript</h2>\n" +
	"<p>Le langage de script dans sa globalité est très similaire au très connu langage Javascript utilisé en programmation web. Il y a cependant quelques différences que j'ai énuméré ci-dessous.</p>\n" +
	"<h3 id=\"point-virgule-obligatoire\">Point-virgule obligatoire</h3>\n" +
	"<p>En javascript il est possible dans certains cas d'omettre le point-virgule en fin d'intruction si l'on fait un retour à la ligne.</p>\n" +
	"<pre><code class=\"javascript\">var a = 1.2\n" +
	"a += 15</code></pre>\n" +
	"<p>Ce code qui fonctionne très bien en Javascript provoquera une erreur en LiteScript, il faudra alors écrire ceci pour que cela fonctionne :</p>\n" +
	"<pre><code class=\"litescript\">var a = 1.2;\n" +
	"a += 15;</code></pre>\n" +
	"<h3 id=\"pas-de-prototype\">Pas de prototype</h3>\n" +
	"<p>Il est possible en Javascript d'accéder au membre &quot;<strong>prototype</strong>&quot; d'une variable pour en modifier le comportement, cet aspect est totalement remplacé par <a href=\"#les-classes\">les classes</a> comme utilisé dans des langages comme le Java ou le C#.</p>\n" +
	"<h3 id=\"fonctions-fléchées\">Fonctions fléchées</h3>\n" +
	"<p>Les fonctions fléchées utlisée en Javascript ne sont pas implémentées en LiteScript, seules les fonctions anonymes sont présentes.</p>\n" +
	"<p>Exemple de fonction fléchée :</p>\n" +
	"<pre><code class=\"javascript\">var a = (a, b) =&gt; { return a % b; };</code></pre>\n" +
	"<h3 id=\"mot-clés-préservés\">Mot-clés préservés</h3>\n" +
	"<p>Les mots-clés présents en Javascript qui sont présents en LiteScript sont les suivants :</p>\n" +
	"<ul>\n" +
	"<li><strong>undefined</strong></li>\n" +
	"<li><strong>null</strong></li>\n" +
	"<li><strong>true</strong></li>\n" +
	"<li><strong>false</strong></li>\n" +
	"<li><strong>new</strong></li>\n" +
	"<li><strong>var</strong></li>\n" +
	"<li><strong>if</strong></li>\n" +
	"<li><strong>else</strong></li>\n" +
	"<li><strong>for</strong></li>\n" +
	"<li><strong>while</strong></li>\n" +
	"<li><strong>do</strong></li>\n" +
	"<li><strong>switch</strong></li>\n" +
	"<li><strong>case</strong></li>\n" +
	"<li><strong>default</strong></li>\n" +
	"<li><strong>break</strong></li>\n" +
	"<li><strong>continue</strong></li>\n" +
	"<li><strong>function</strong></li>\n" +
	"<li><strong>this</strong></li>\n" +
	"<li><strong>return</strong></li>\n" +
	"</ul>\n" +
	"<p>Les mots-clés supplémentaires dont certains sont réservés en Javascript avec le <strong><em>strict mode</em></strong> sont les suivants :</p>\n" +
	"<ul>\n" +
	"<li><strong>class</strong></li>\n" +
	"<li><strong>static</strong></li>\n" +
	"<li><strong>operator</strong></li>\n" +
	"<li><strong>namespace</strong></li>\n" +
	"</ul>";

content["1.0"]["fr"]["récupérer-les-erreurs-de-syntaxe"] = "" + 
	"<h1 id=\"récupérer-les-erreurs-de-syntaxe\">Récupérer les erreurs de syntaxe</h1>\n" +
	"<p>Avant d'exécuter un script, il est nécessaire de transformer un code source textuel en liste d'instructions (semi-compilation). Pendant cette phase, il est possible de rencontrer une erreur de syntaxe dans le code source, ce qui aura pour effet de stopper la semi-compilation du code. Hors il faut être capable de vérifier si celle-ci c'est bien déroulée avant d'exécuter les instructions. Il faudra alors procéder de la manière suivante.</p>\n" +
	"<p>Dans le cas d'un script classique :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Code source du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state;\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Script script(state);\n" +
	"    // Exécution du code\n" +
	"    script.Execute(code);\n" +
	"    // Vérification de la non présence d'erreur\n" +
	"    if (script.error != Script::ErrorType::SCRPT_ERROR_NO) {\n" +
	"        // Sinon affichage d'un message d'erreur\n" +
	"        std::cout &lt;&lt; \"ERROR(\" &lt;&lt; (script.line_error + 1) &lt;&lt; \",\" &lt;&lt; (script.col_error + 1) &lt;&lt; \"): \" &lt;&lt; script.GetError() &lt;&lt; std::endl;\n" +
	"        return -1;\n" +
	"    }\n" +
	"    return 0;\n" +
	"}\n" +
	"\n" +
	"/* OU ALORS\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state;\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Script script(state);\n" +
	"    Script::ErrorType error;\n" +
	"    unsigned int line_error, col_error;\n" +
	"    // Récupération de la liste d'instruction\n" +
	"    std::vector&lt;Instruction&gt; instructions = Script::GetInstructionList(code, error, line_error, col_error);\n" +
	"    // Vérification de la non présence d'erreur\n" +
	"    if (error != Script::ErrorType::SCRPT_ERROR_NO) {\n" +
	"        // Sinon affichage d'un message d'erreur\n" +
	"        std::cout &lt;&lt; \"ERROR(\" &lt;&lt; (line_error + 1) &lt;&lt; \",\" &lt;&lt; (col_error + 1) &lt;&lt; \"): \" &lt;&lt; script.GetError() &lt;&lt; std::endl;\n" +
	"        return -1;\n" +
	"    }\n" +
	"    // Ajout des instructions dans l'état du script\n" +
	"    state.AddInstructions(instructions);\n" +
	"    // Exécution des instructions\n" +
	"    state.Execute();\n" +
	"    return 0;\n" +
	"}\n" +
	"\n" +
	"*/</code></pre>\n" +
	"<p>Et dans le cas d'un script assembleur :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Code source du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state;\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Assembly script(state);\n" +
	"    // Exécution du code\n" +
	"    script.Execute(code);\n" +
	"    // Vérification de la non présence d'erreur\n" +
	"    if (script.error != Script::ErrorType::SCRPT_ERROR_NO) {\n" +
	"        // Sinon affichage d'un message d'erreur\n" +
	"        std::cout &lt;&lt; \"ERROR(\" &lt;&lt; (script.line_error + 1) &lt;&lt; \"): \" &lt;&lt; script.GetError() &lt;&lt; std::endl;\n" +
	"        return -1;\n" +
	"    }\n" +
	"    return 0;\n" +
	"}\n" +
	"\n" +
	"/* OU ALORS\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state;\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Script script(state);\n" +
	"    Script::ErrorType error;\n" +
	"    unsigned int line_error;\n" +
	"    // Récupération de la liste d'instruction\n" +
	"    std::vector&lt;Instruction&gt; instructions = Assembly::GetInstructionList(code, error, line_error);\n" +
	"    // Vérification de la non présence d'erreur\n" +
	"    if (error != Script::ErrorType::SCRPT_ERROR_NO) {\n" +
	"        // Sinon affichage d'un message d'erreur\n" +
	"        std::cout &lt;&lt; \"ERROR(\" &lt;&lt; (line_error + 1) &lt;&lt; \"): \" &lt;&lt; script.GetError() &lt;&lt; std::endl;\n" +
	"        return -1;\n" +
	"    }\n" +
	"    // Ajout des instructions dans l'état du script\n" +
	"    state.AddInstructions(instructions);\n" +
	"    // Exécution des instructions\n" +
	"    state.Execute();\n" +
	"    return 0;\n" +
	"}\n" +
	"\n" +
	"*/</code></pre>";

content["1.0"]["fr"]["sauvegarder-dans-des-fichiers"] = "" + 
	"<h1 id=\"sauvegarder-dans-des-fichiers\">Sauvegarder dans des fichiers</h1>\n" +
	"<p>Il est possible pour ce moteur de script de sauvegarder plusieurs éléments dans des fichiers, à savoir les instructions, la mémoire et l'état du script.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"sauvegarder-les-instructions\">Sauvegarder les instructions</h2>\n" +
	"<p>Il est possible d'enregistrer des instructions élémentaires dans un flux textuel ou binaire.</p>\n" +
	"<p>Dans le cas du flux textuel, il faut procéder comme suit :</p>\n" +
	"<pre><code class=\"cpp\">#include &lt;fstream&gt;\n" +
	"#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    std::vector&lt;Instruction&gt; instructions;\n" +
	"\n" +
	"    /* RECUPERATION D'INSTRUCTIONS */\n" +
	"\n" +
	"    // Ouverture du fichier dans lequel écrire\n" +
	"    std::ofstream file(\"fichier.txt\");\n" +
	"    if (!file.is_open())\n" +
	"        return -1;\n" +
	"\n" +
	"    // Ecriture des instructions\n" +
	"    Instruction::Save(file, instructions);\n" +
	"\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p>Et pour les charger :</p>\n" +
	"<pre><code class=\"cpp\">#include &lt;fstream&gt;\n" +
	"#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Ouverture du fichier dans lequel lire\n" +
	"    std::ifstream file(\"fichier.txt\");\n" +
	"    if (!file.is_open())\n" +
	"        return -1;\n" +
	"\n" +
	"    // Lecture des instructions\n" +
	"    std::vector&lt;Instruction&gt; instructions = Instruction::Load(file);\n" +
	"\n" +
	"    /* etc... */\n" +
	"\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p>Dans le cas du flux binaire, la méthode est la même, la seule différence est qu'il faut utiliser les méthodes statiques &quot;<strong>SaveBinary</strong>&quot; et &quot;<strong>LoadBinary</strong>&quot; à la place de &quot;<strong>Save</strong>&quot; et &quot;<strong>Load</strong>&quot; de la structure <a href=\"#la-structure-instruction\">Instruction</a>.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"sauvegarder-la-mémoire\">Sauvegarder la mémoire</h2>\n" +
	"<p>Il est possible d'enregistrer et de charger la mémoire dans un fichier en faisant comme suit :</p>\n" +
	"<pre><code class=\"cpp\">#include &lt;fstream&gt;\n" +
	"#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // On ouvre le fichier contenant la mémoire\n" +
	"    std::ifstream file_in(\"memory\", std::ios::binary);\n" +
	"    if (!file_in.is_open())\n" +
	"        return -1;\n" +
	"\n" +
	"    // On charge la mémoire\n" +
	"    Memory memory = Memory::Load(file_in);\n" +
	"\n" +
	"    /* TRAITEMENT SUR LA MEMOIRE */\n" +
	"\n" +
	"    // On ouvre le fichier pour y sauvegarder la mémoire\n" +
	"    std::ofstream file_out(\"memory\", std::ios::binary);\n" +
	"    if (!file_out.is_open())\n" +
	"        return -1;\n" +
	"\n" +
	"    // On sauvegarde la mémoire\n" +
	"    Memory::Save(file_out, memory);\n" +
	"\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p><strong><em>Remarque</em></strong> : Si vous utilisez la sauvegarde de la mémoire, il est important que les fonctions internes soient assignées dans la liste d'appels de la classe <a href=\"#la-classe-callback\">Callback</a> dans le même ordre à la lecture de la mémoire qu'à sa sauvegarde. Dans ce cas là, il est déconseillé d'utiliser la fonction <strong>CreateVariable</strong> pour créer une variable de type &quot;Callback&quot;, mais plutôt le code suivant :</p>\n" +
	"<pre><code class=\"cpp\">// On enregistre l'index de la fonction dans la liste\n" +
	"unsigned int callID = AddCallback(maFonction);\n" +
	"\n" +
	"/* OU ALORS SANS LES FONCTIONS UTILES\n" +
	"\n" +
	"unsigned int callID = Callback::List.size();\n" +
	"Callback::List.push_back(maFonction);\n" +
	"\n" +
	"*/\n" +
	"\n" +
	"Variable mon_appel = CreateCallback(memory, callID);</code></pre>\n" +
	"<p>Vous pouvez arranger la liste <a href=\"#la-classe-callback\">Callback</a>::<strong>List</strong> comme vous le souhaitez, cependant l'index de l'appel est enregistré à la création de la <a href=\"#litescript__variable\">Variable</a> et n'est pas modifiable dans la classe <a href=\"#la-classe-callback\">Callback</a>, ce qui veut dire qu'au moment de la sauvegarde c'est cet index qui est enregistré dans le flux de données.</p>\n" +
	"<hr />\n" +
	"<h2 id=\"sauvegarder-létat-du-script\">Sauvegarder l'état du script</h2>\n" +
	"<p>Enfin il est possible de charger et de sauvegarder l'état d'un script en mémoire comme suit :</p>\n" +
	"<pre><code class=\"cpp\">#include &lt;fstream&gt;\n" +
	"#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"\n" +
	"    // Chargement des instructions et de la mémoire ...\n" +
	"\n" +
	"    // On charge l'état du script\n" +
	"    State state = State::Load(file, memory);\n" +
	"    // Puis on ré-insère après coup les instructions\n" +
	"    state.AddInstructions(instructions);\n" +
	"\n" +
	"    /* TRAITEMENT QUELCONQUE */\n" +
	"\n" +
	"    // On sauvegarde l'état du script dans un fichier\n" +
	"    State::Save(file, state);\n" +
	"\n" +
	"    return 0;\n" +
	"}</code></pre>\n" +
	"<p><strong><em>Attention</em></strong> : lorsque vous ré-insérez les instructions, n'utilisez pas la méthode <strong>AddInstruction</strong> qui ajoute une seule instruction car cela risquerait de modifier les attributs &quot;<strong>instr_index</strong>&quot; et &quot;<strong>line_num</strong>&quot; s'ils sont positionnés sur des instructions qui n'existent pas encore.</p>";

content["1.0"]["fr"]["utiliser-le-ramasse-miettes"] = "" + 
	"<h1 id=\"utiliser-le-ramasse-miettes\">Utiliser le ramasse-miettes</h1>\n" +
	"<p>Le moteur de script possède un système de gestion dynamique de la mémoire, c'est à dire que les objets qui ne sont plus référencées par des variables sont automatiquement supprimés de la mémoire mais ce système a quelques limites, notamment lorsque deux objets isolés contiennent des références mutuelles.</p>\n" +
	"<hr />\n" +
	"<p>Il est donc possible d'utiliser une fonction &quot;Ramasse-miettes&quot; pour supprimer de la mémoire des variables isolées. Cependant il est important de prendre en compte plusieurs choses.</p>\n" +
	"<ol>\n" +
	"<li>Les objets référencés uniquement par des variables en dehors de l'état de script et ces espaces de noms est potenciellement susceptible d'être supprimé lors de l'exécution de la fonction &quot;Ramasse-miettes&quot;. Par exemple :</li>\n" +
	"</ol>\n" +
	"<pre><code class=\"cpp\">Variable MaFonction(State&amp; state, std::vector&lt;Variable&gt;&amp; args) {\n" +
	"    Variable danger = CreateVariable(state.memory, \"Il y a un danger !\");\n" +
	"    state.memory.GarbageCollector(state);\n" +
	"    return danger;\n" +
	"}</code></pre>\n" +
	"<p>Dans cet exemple, la variable &quot;<strong>danger</strong>&quot; est créée mais n'est pas contenue dans les espaces de noms de l'état du script, ainsi lors de l'exécution du <strong><em>ramasse-miettes</em></strong> l'objet qui est référencé est supprimé et donc provoquera très probablement une erreur plus tard.</p>\n" +
	"<ol start=\"2\">\n" +
	"<li>Il faut éviter d'utiliser cette fonction trop souvent car sur des scripts lourds son exécution peut être gourmande, il est donc conseillé de l'utiliser avec un certain interval de temps (~ 1 seconde, voir plus) ou à la fin de chaque <strong><em>update</em></strong> dans le cas de scripts avec une fonction principale.</li>\n" +
	"</ol>\n" +
	"<p>Voici comment utiliser le <strong><em>ramasse-miettes</em></strong> :</p>\n" +
	"<pre><code class=\"cpp\">#include \"litescript.hpp\"\n" +
	"\n" +
	"using namespace LiteScript;\n" +
	"\n" +
	"// Contenu du script à exécuter\n" +
	"char * code = \"Code source\";\n" +
	"\n" +
	"int main(int argc, char * argv[]) {\n" +
	"    // Mémoire principale\n" +
	"    Memory memory;\n" +
	"    // Etat du script\n" +
	"    State state(memory);\n" +
	"    // Classe d'exécution simplifiée\n" +
	"    Script script(state);\n" +
	"    // Exécution du code\n" +
	"    script.Execute(code);\n" +
	"    while (true) {\n" +
	"\n" +
	"        /* UPDATE DU SCRIPT */\n" +
	"\n" +
	"        // Exécution du ramasse-miettes\n" +
	"        memory.GarbageCollector(state);\n" +
	"    }\n" +
	"    return 0;\n" +
	"}</code></pre>";

content["1.0"]["fr"]["404"] = "" + 
	"<h1 id=\"page-introuvable\">Page introuvable</h1>\n" +
	"<p>La page à laquelle vous souhaitez accéder est introuvable ou est inaccessible.</p>";