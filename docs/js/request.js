var Request = {
    lang: "fr",
    version: "1.0"
};
if (window.location.search !== "") {
    var get = window.location.search.substr(1).split('&');
    for (var i = 0; i < get.length; i++) {
        var tmp = get[i].split('=');
        get[tmp[0]] = tmp[1] !== undefined ? tmp[1] : "";
    }
    if (get['lang'] !== undefined)
        Request.lang = get['lang'];
    if (get['version'] !== undefined)
        Request.version = get['version'];
}
var content = {};
var lang;
switch (Request.lang) {
    case 'en':
        lang = {
            search: "Search",
            footer: "Documentation",
            versions: "Versions",
            github: "Github project",
            view: "View",
            languages: "Languages",
            copyright: "Using |($1)Sphinx theme| for |($2)Read the Docs|.",
            next: "Next",
            previous: "Previous",
            search_success: "Search results",
            search_fail: "No result"
        };
        break;
    case 'es':
        lang = {
            search: "Buscar",
            footer: "Documentación",
            versions: "Versiones",
            github: "Proyecto Github",
            view: "Ver",
            languages: "Lenguas",
            copyright: "Uso del |($1)tema Sphinx| para |($2)Read the Docs|.",
            next: "Próximo",
            previous: "Anterior",
            search_success: "Resultados de Búsqueda",
            search_fail: "Ningún resultado"
        };
        break;
    case 'ja':
        lang = {
            search: "検索",
            footer: "ドキュメンテーション",
            versions: "バージョン",
            github: "プロジェクトGithub",
            view: "ビュー",
            languages: "言語",
            copyright: "|($2)Read the Docs|に|($1)Sphinxテーマ|を使用する。",
            next: "以下",
            previous: "前",
            search_success: "検索結果",
            search_fail: "結果はありません"
        };
        break;
    default:
        lang = {
            search: "Rechercher",
            footer: "Documentation",
            versions: "Versions",
            github: "Projet Github",
            view: "Voir",
            languages: "Langues",
            copyright: "Utilisation du |($1)thème de Sphinx| pour |($2)Read the Docs|.",
            next: "Suivant",
            previous: "Précédent",
            search_success: "Résultats de la recherche",
            search_fail: "Aucun résultat"
        };
}
lang['code'] = {
    fr: "Français",
    en: "English",
    es: "Español",
    ja: "日本語"
};
function URLGet(get) {
    if (get.length === 0)
        return "";
    var res = "?";
    $.each(get, function (key, item) {
        if (res.length > 1)
            res += "&";
        res += key + "=" + item;
    });
    return res;
}
$(document).ready(function () {
    $('.search-container input').attr('placeholder', lang.search);
    $('.rst-current-version .fa.fa-book').text(" " + lang.footer);
    $('.rst-current-version .vers-num').text(Request.version);
    $('.dl-version dt').text(lang.versions);
    $('.dl-github dt').text(lang.github);
    $('.dl-lang dt').text(lang.languages);
    $('.dl-footer').html((function () {
        var res = "";
        $.each(lang.copyright.split('|'), function (key, item) {
            var sub = item.substr(0, 4);
            if (sub === "($1)")
                res += '<a href="https://github.com/snide/sphinx_rtd_theme">' + item.substr(4) + '</a>';
            else if (sub === "($2)")
                res += '<a href="https://readthedocs.org/">' + item.substr(4) + '</a>';
            else if (item)
                res += item;
        });
        return res;
    })());
    $.each(content, function (key, item) {
        if (key === Request.version) {
            $('.dl-version').append($('<strong><dd><a>' + key + '</a></dd></strong>'));
            $.each(content[key], function (key, item) {
                if (key === Request.lang)
                    $('.dl-lang').append($('<strong><dd><a>' + lang['code'][key] + '</a></dd></strong>'));
                else
                    $('.dl-lang').append($('<dd><a href="' + URLGet(Object.assign({}, Request, { lang: key })) + '">' + lang['code'][key] + '</a></dd>'));
            });
        }
        else
            $('.dl-version').append($('<dd><a href="' + URLGet(Object.assign({}, Request, { version: key })) + '">' + key + '</a></dd>'));
    });
    $('.rst-footer-buttons .next').text(lang.next);
    $('.rst-footer-buttons .prev').text(lang.previous);
});