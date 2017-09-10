$(document).ready(function () {
	$('.rst-content .document .section').html($(content[Request.version][Request.lang]['index']));
	
    $('.rst-current-version').click(function () {
        $('.rst-versions').toggleClass('shift-up');
    });
    $('.fa.fa-bars').click(function () {
        $('.wy-nav-side').toggleClass('shift');
        $('.rst-versions').toggleClass('shift');
        $('.wy-nav-content-wrap').toggleClass('shift');
    });

    var main_content = $('.document .section');
    var summary = main_content.find('hr').nextAll('h2');
    function getCategory(e) {
        var li = $(e).children('li').children('a');
        var result = [];
        for (var i = 0; i < li.length; i++) {
            result[i] = {
                name: li[i].innerText,
                href: li[i].getAttribute('href'),
                child: null
            };
            $(li[i]).next('ul').each(function () {
                result[i].child = getCategory(this);
            });
        }
        return result;
    }
    var categories = [];
    for (var i = 0; i < summary.length; i++) {
        categories[i] = {
            name: summary[i].innerText,
            child: null
        };
        $(summary[i]).nextUntil('h2').each(function () {
            categories[i].child = getCategory(this);
        });
    }

    var nav = $('.wy-menu.wy-menu-vertical');
    function setCategory(child, level) {
        var result = $('<ul>');
        child.forEach(function (e) {
            var tmp = $('<li class="toctree-l' + level + '">');
            if (e.child !== null) {
                tmp.append($('<a class="reference internal" href="' + e.href + '"><span class="toctree-expand"></span>' + e.name + '</a>'));
                tmp.append(setCategory(e.child, level + 1));
            }
            else
                tmp.append($('<a class="reference internal" href="' + e.href + '">' + e.name + '</a>'));
            result.append(tmp);
        });
        return result;
    }
    categories.forEach(function (e) {
        nav.append($('<p class="caption"><span class="caption-text">' + e.name + '</span></p>'));
        if (e.child !== null)
            nav.append(setCategory(e.child, 1));
    });

    var title = $('.document .section h1').first().text();
    $('.wy-breadcrumbs').children('.title').text(title);

    function reloadHighlight() {
        $('.wy-nav-content pre code').each(function(i, block) {
            hljs.highlightBlock(block);
        });
    }
    function loadCategory(name, replace) {
        if (content[Request.version][Request.lang][name] !== undefined)
            $('.rst-content .document .section').html($(content[Request.version][Request.lang][name]));
        else
            $('.rst-content .document .section').html($(content[Request.version][Request.lang]['404']));
        var title = $('.document .section h1').first().text();
        $('.wy-breadcrumbs').children('.title').text(title);
        reloadHighlight();
        $('.wy-nav-side').toggleClass('shift', false);
        $('.rst-versions').toggleClass('shift', false);
        $('.wy-nav-content-wrap').toggleClass('shift', false);
        if (replace !== false)
            window.location.replace(window.location.href);
    }
    function searchPrevious() {
        var node = $('.wy-menu.wy-menu-vertical').children('ul.current');
        if (node.length === 0)
            return null;
        var li = node.children('li.current').prev('li');
        if (li.length === 0) {
            li = node.prev().prev('ul').children('li');
            if (li.length !== 0)
                li = $(li[li.length - 1]);
        }
        if (li.length === 0)
            return "";
        else
            return li.children('a').attr('href');
    }
    function searchNext() {
        var node = $('.wy-menu.wy-menu-vertical').children('ul.current');
        var li = node.children('li.current').next('li');
        if (node.length === 0) {
            li = $('.wy-menu.wy-menu-vertical').children('ul').first().children('li').first();
        }
        if (li.length === 0) {
            li = node.next().next('ul').children('li');
            if (li.length !== 0)
                li = $(li[0]);
        }
        if (li.length === 0)
            return null;
        else
            return li.children('a').attr('href');
    }
    function reloadButtons() {
        var buttons = $('.rst-footer-buttons').children();
        var tmp;
        if ((tmp = searchPrevious()) === null)
            $(buttons[1]).hide();
        else
            $(buttons[1]).show().attr('href', tmp);
        if ((tmp = searchNext()) === null)
            $(buttons[0]).hide();
        else
            $(buttons[0]).show().attr('href', tmp);
    }
    function searchCategory(node, hash) {
        var nd = $(node);
        var a = nd.children('a');
        if (a.attr('href') === hash) {
            a.addClass('current');
            a.parent().addClass('current');
            return true;
        }
        var ul = nd.children('ul');
        if (ul.length > 0) {
            var success = false;
            ul.children('li').each(function () {
                if (searchCategory(this, hash)) {
                    success = true;
                    return false;
                }
            });
            if (success) {
                a.addClass('current');
                return true;
            }
        }
        return false;
    }
    function searchCategories() {
        $('.wy-menu.wy-menu-vertical .current').toggleClass('current');
        $('.wy-menu.wy-menu-vertical').children('ul').each(function () {
            var success = false;
            $(this).children('li').each(function () {
                if (searchCategory(this, window.location.hash)) {
                    $(this).addClass('current');
                    success = true;
                    return false;
                }
            });
            if (success) {
                $(this).addClass('current');
                return false;
            }
        });
        var hash = $('.wy-menu.wy-menu-vertical').children('.current').children('.current').children('a.current').attr('href');
        if (hash[0] == '#')
            hash = hash.substr(1);
        loadCategory(hash);
        reloadButtons();
    }
    if (window.location.hash !== '')
        searchCategories();
    else {
        reloadButtons();
        reloadHighlight();
    }
    $(window).on('hashchange', function(e){
		if (window.location.hash === '') {
			$('.wy-menu.wy-menu-vertical .current').toggleClass('current');
			loadCategory('index', false);
			reloadButtons();
		}
		else
			searchCategories();
    });

    function searchKeywords(text) {
        history.pushState('', document.title, window.location.pathname);

        var corresp = [];
        var regex = new RegExp(text.replace(/\s+/, "|"), 'ig');
        $.each(content[Request.version][Request.lang], function (key, item) {
            if (key === 'index')
                return true;
            var node = $(item);
            var title = node.filter('h1').text();
            var parag = node.filter('p');
            const maxlength = 200;
            var e = {
                hash: key,
                title: title,
                tfounded: false,
                parag: $(parag[0]).text().substr(0, maxlength),
                pfounded: false
            };
            if (title.match(regex))
                e['tfounded'] = true;
            parag.each(function(key, item) {
                var tmp = item.innerText.search(regex);
                if (tmp !== -1) {
                    e['pfounded'] = true;
                    var pos = tmp > maxlength ? tmp - maxlength / 2 : 0;
                    e['parag'] = "";
                    if (pos > 0)
                        e['parag'] = "...";
                    e['parag'] += item.innerText.substr(pos, maxlength);
                    if (item.innerText.length > pos + maxlength)
                        e['parag'] += "...";
                    return false;
                }
            });
            if (e['tfounded'] || e['pfounded'])
                corresp.push(e);
			else {
				node.filter('*').each(function (key, item) {
					if (item.innerText.search(regex) !== -1) {
						corresp.push(e);
						return false;
					}
				});
			}
        });
        corresp.sort(function(a, b) {
            if (a['tfounded']) {
                if (!b['tfounded'])
                    return -1;
            }
            else {
                if (b['tfounded'])
                    return 1;
            }
            if (a['pfounded']) {
                if (b['pfounded'])
                    return 0;
                else
                    return -1;
            }
            else {
                if (b['pfounded'])
                    return 1;
                else
                    return 0;
            }
        });
        var result = $('<div class="section"></div>');
        if (corresp.length > 0)
            result.append($('<h1>' + lang['search_success'] + '</h1>'));
        else
            result.append($('<h1>' + lang['search_fail'] + '</h1>'));
        function keywHighlt(match) {
            return '<span class="keyword-highlight">' + match + '</span>';
        }
        $.each(corresp, function (key, item) {
            result.append($('<h2><a href="#' + item.hash + '">' + item.title.replace(regex, keywHighlt) + '</a></h2>'));
            result.append($('<p>' + item.parag.replace(regex, keywHighlt) + '</p>'))
        });
        $('.rst-content .document').html(result);
        $('.wy-breadcrumbs').children('.title').text(lang['search_success']);
        $('.wy-menu.wy-menu-vertical .current').toggleClass('current');
        $('.rst-footer-buttons').children().hide();
        $('.wy-nav-side').toggleClass('shift', false);
        $('.rst-versions').toggleClass('shift', false);
        $('.wy-nav-content-wrap').toggleClass('shift', false);
    }
    $('.wy-side-nav-search input').keypress(function (e) {
        if (e.keyCode === 13) {
            var value = $('.wy-side-nav-search input').val();
            if (value !== '')
                searchKeywords(value);
        }
    });
});