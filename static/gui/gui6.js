/*
 GUI.js 

The MIT License (MIT)

Copyright (c) 2013 RÃ©mi Arnaud - Advanced Micro Devices, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 GUI.js needs jquery, jqueryUI and jqueryUI layout
*/
"use strict";

if (window.$ === undefined) {
  //document.write('<link rel="stylesheet" href="/gui/themes/vader/jquery-ui.css" />');
	document.write('<link rel="stylesheet" href="/gui/themes/custom-theme/jquery-ui-1.10.3.custom.css" />');
	//document.write('<link rel="stylesheet" href="/gui/themes/dot-luv/jquery-ui-1.10.3.custom.css" />');
	//document.write('<link rel="stylesheet" href="/gui/themes/dark-hive/jquery-ui-1.10.3.custom.css" />');

    document.write('<script src="/deps/jquery.js"><\/' + 'script>');
    document.write('<script src="/deps/jquery-ui.js"><\/' + 'script>');
    document.write('<script src="/deps/jquery.layout.min.js"><\/' + 'script>');

    document.write('<script type="text/javascript" src="/deps/jquery.cookie.js"><\/' + 'script>');
    document.write('<script type="text/javascript" src="/deps/jquery.hotkeys.js"><\/' + 'script>');
    document.write('<script type="text/javascript" src="/deps/jquery.jstree.js"><\/' + 'script>');
    document.write('<script type="text/javascript" src="/deps/jquery.terminal-0.7.3.js"><\/' + 'script>');
    document.write('<script type="text/javascript" src="/deps/jquery.toolbar.js"><\/' + 'script>');
    document.write('<script type="text/javascript" src="/deps/console.js"><\/' + 'script>');
    //document.write('<link href="/gui/themes/vader/ui.dynatree.css" rel="stylesheet" type="text/css" />');

    document.write('<link rel="stylesheet" href="/gui/gui6.css" />');
    document.write('<link rel="stylesheet" href="/gui/themes/jquery.terminal.css" />');
    document.write('<link rel="stylesheet" href="/gui/themes/jquery.toolbars.css" />');
    document.write('<link rel="stylesheet" href="/gui/themes/bootstrap.icons.css" />');



}



/***
  Require Underscore, if we're on the server, and it's not already present.
  var _ = root._;
  if (!_ && (typeof require !== 'undefined')) _ = require('underscore')._;
***/
// UI library
(function () {


    // Initial Setup
    // -------------
    // Save a reference to the global object (`window` in the browser, `exports`
    // on the server).

    // The top-level namespace. All public GUI classes and modules will
    // be attached to this. Exported for both CommonJS and the browser.
    var GUI;
    if (typeof exports !== 'undefined') GUI = exports;
    else {
        GUI = this.GUI = {};
    };
    // Current version of the library. Keep in sync with `package.json`.
    GUI.VERSION = '0.0.1';

    GUI.bufferLayout = [];

    GUI.button = function (_txt, _parent, _callback, _x1, _y1, _x2, _y2) {
        var $button = $('<button></button>');
        var callback = _callback;

        $button.button().click(function (event) {
            if (callback) callback.call(this);
            event.preventDefault();
        });
        // create unique ID
        $button.button().uniqueId();
        var id = $button.attr('id');
        if (_txt !== undefined && _txt !== null)
            $button.button({
                label: _txt
            });
        if (_x1 !== undefined && _y1 !== undefined) {
            $button.css('position', 'absolute');
            $button.css('left', _x1);
            $button.css('top', _y1);
            if (_y2 !== undefined && _y1 !== undefined) {
                _y2 -= _y1;
                _x2 -= _x1;
            }
        }
        if (_y2 !== undefined && _y1 !== undefined) {
            $button.css('height', _y2);
            $button.css('width', _x2);
        }

        if (!_parent)
            $('body').append($button);
        else
            $(_parent).append($button);
        return $button
    };

    GUI.label = function (_id, _txt, _parent, _style, _mode) {
        var label = '';
        if (_mode == "isolate") {
            label = '<p';
            if (_id) label += ' id= "p' + _id;
            label += '" >';
        }
        label += '<span style="' + _style;
        if (_id) label += '" id= "' + _id;
        label += '">' + _txt + '</span>';
        if (_mode == "isolate") {
            label += '</p>';
        }
        var $label = $(label);
        //$label.button().uniqueId();
        if (!_parent)
            $('body').append($label);
        else if (_mode == "replace")
            $(_parent).appendTo($label);
        else
            $(_parent).append($label);
        return $label;
    };

    GUI.tree = function (_tree, _parent, _callback, _id) {
        var cb = _callback;
        var tree = '<div';
        if (_id) tree += ' id=' + _id;
        tree += '> </div>';
        var $tree = $(tree);
        if (!_parent)
            $('body').append($tree);
        else
            $(_parent).append($tree);


        $tree.jstree({
            plugins: ["dnd", "ui", "themeroller", "json_data", "crrm", "types"],
            animation: 0,
            /* ONLY FOR FOREIGN NODES
          dnd : {
            drag_target : ".jstree-draggable",
            drop_target : ".jstree-drop",
            drag_check : function (data) {
               if(data.r.attr("id") == "phtml_1") {
                  return false;
               }
               return {
                after : false,
                before : false,
                inside : true
               };
            },
            drop_check : function (data) {
                return false;
            },
            drop_finish : function () {
               alert("DROP");
            },
            drag_finish : function (data) {
                alert("DRAG OK");
            },
          },
            */
            crrm: {
                move: {
                    default_position: "inside",
                    always_copy: "multitree",
                    check_move: function (m) {
                        var ret = false;
                        // we can put a model in a scene
                        if (m.r.attr('rel') === 'scene' &&
                            (m.o.attr('rel') === 'model'))
                            ret = true;
                        return ret;

                    }
                }
            },

            types: {
                max_depth: -2,
                max_children: -2,
                valid_children: ["root", "scene"],
                types: {
                    root: {
                        icon: {
                            image: "/gui/img/drive.png"
                        },
                        valid_children: ["collection"],

                        hover_node: false,
                        //select_node : function () {return false;}
                    },
                    scene: {
                        icon: {
                            image: "/gui/img/scene-root22.png"
                        },
                        valid_children: ["model"],
                        //select_node : function () {return false;}
                    },
                    collection: {
                        icon: {
                            image: "/gui/img/folder.png"
                        },
                        valid_children: ["image", "shader", "model"],

                        hover_node: true,
                        //select_node : function () {return false;}
                    },
                    image: {
                        icon: {
                            image: "/gui/img/media-image.png"
                        },
                        valid_children: "none",
                    },
                    shader: {
                        icon: {
                            image: "/gui/img/GLSLStudioIcon22.png"
                        },
                        valid_children: "none",
                    },
                    model: {
                        icon: {
                            image: "/gui/img/bunny22.png"
                        },
                        valid_children: "none",
                    },
                    default: {
                        valid_children: ["default"]
                    }
                }
            },
            json_data: {
                data: [_tree]
            }
        }).bind("select_node.jstree", function (e, data) { // requires ui plugin
            // `data.rslt.obj` is the jquery extended node that was clicked
            if (cb) cb.call(data.inst, e, data.rslt);
            data.inst.deselect_node(data.rslt.obj);
        }).bind("move_node.jstree", function (e, data) { // requires crrm plugin
            if (cb) cb.call(data.inst, e, data.rslt);
        }).bind("dblclick.jstree", function (e, data) {
            //var node = $(e.target).closest("li");
            //var id = node[0].id; //id of the selected node
            if (cb) cb.call(data.inst, e, data.rslt);
        }).bind("create_node.jstree", function (e, data) {
            if (cb) cb.call(data.inst, e, data.rslt);
        }).bind("open_node.jstree", function (e, data) {
            if (cb) cb.call(data.inst, e, data.rslt);
        });

        return $tree;
    };

    GUI.canvas = function (_parent) {
        $canvas = $('<canvas class="ui-resize-me" style="padding: 0; margin: 0;" ></canvas>');

        resize = function (event) {
            $(this).height($(this).parent().parent().height() - 2);
            $(this).width($(this).parent().parent().width());
            if (this.width != this.clientWidth ||
                this.height != this.clientHeight) {
                // Change the size of the canvas to match the size it's being displayed
                this.width = this.clientWidth;
                this.height = this.clientHeight;
            }
        };

        //$canvas.button().uniqueId();
        if (!_parent)
            $('body').append($canvas);
        else {

            $parent = $(_parent);
            $parent.append($canvas);
            resize.call($canvas[0], null);
        }

        $canvas.bind('resize', resize);

        return $canvas[0];
    };

    //----------------------------------------------------------------------------------------------------------------------------------------
    GUI.autoScroll = function (_jqueryObject, _parentToScroll) {
        _jqueryObject.on("click", function (event, ui) {
            setTimeout(function () {
                _parentToScroll.scrollTop((_parentToScroll[0].scrollHeight + 500) - (_parentToScroll.height()))
            }, 300);
        });
    };

    GUI.time = function () {
        var html = "<a id='time'> ";
        var a = new Date();
        var myDate = a.toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1");
        var ms = '.' + String((a.getUTCMilliseconds() / 1000).toFixed(3)).slice(2, 5)
        html += myDate + ms;
        html += "</a>";
        return html;
    }

    GUI.uniqueId = function () {
        var html = "ui-id-"+Math.round(Math.random()*10000);
        return html;
    }

    //checked	 
    GUI.addAccordion = function (_id, _items, _parent, _content, _mode) {
        if (_id == "console" || _id == "Console") return console.error("GUI.console: _id ever used, please enter a new one");
        var size_items = _items.length;
        var accordion = '<div id="' + _id + '" >'
        for (i = 0; i < size_items; i++) {
            accordion += '<h3>' + _items[i] + '</h3><div id="' + _id + '-' + i + '" >';
            if (_content && (_content[i] != "0")) {
                accordion += _content[i];
            }
            accordion += '</div>';
        }
        accordion += '</div>';
        var $accordion = $(accordion);
        if (_mode == "before") {
            _parent.prepend($accordion);
        } else _parent.append($accordion);
        $('#' + _id).accordion({
            header: "h3",
            navigation: true,
            collapsible: true,
            active: false,
            heightStyle: "content"
        });
        return $accordion;
    };

    //checked
    GUI.addNewTab = function (_idTabWindow, _idTab, _headerTitle, _contentHTML) {
        var headerTab = '<li><a href="#' + _idTab + '">' + _headerTitle + '</a></li>';
        var $headerTab = $(headerTab);
        $("#header-" + _idTabWindow).append($headerTab);
        var contentTab = '<div id="' + _idTab + '">';
        if (_contentHTML) {
            contentTab += _contentHTML;
        }
        contentTab += '</div>';
        var $contentTab = $(contentTab);
        $("#content-" + _idTabWindow).append($contentTab);
        $("#header-"+_idTabWindow).parent().tabs("refresh");
        return $contentTab;
    }

    //checked
    GUI.addTabWindow = function (_parent, _idTab, _headerTitle, _contentHTML) {
        var tab = '<ul id="header-' + _idTab + '" ><li><a href="#' + _idTab + '">' + _headerTitle + '</a></li></ul>';
        tab += '<div id="content-' + _idTab + '" class="ui-widget-content ui-layout-content ui-corner-top" >';
        tab += '<div id="' + _idTab + '" class="content">';
        if (_contentHTML) {
            tab += _contentHTML;
        }
        tab += '</div></div>'
        var $tab = $(tab);
        _parent.append($tab);
        _parent.tabs();
        return $tab;
    };

    //checked	 	 
    GUI.addSlider = function (_id, _parent, _min, _max, _step, _defaultValue, _style) {
        var slider = '<div id="' + _id + '"'
        if (_style) {
            slider += 'style="' + _style + '" ';
        }
        slider += '></div>';
        var $slider = $(slider);
        _parent.append($slider);
        $("#" + _id).slider({
            animate: true,
            min: _min,
            max: _max,
            step: _step,
            value: _defaultValue
        });
        return $slider;
    };

    //checked
    GUI.addIcon = function (_parent, _cssClass, _style, _position) {
        var icon = '';
        if (_style) {
            icon = '<span id="' + _cssClass + '" class="ui-icon ' + _cssClass + '" style="' + _style + '"';
        } else {
            icon = '<span class="ui-icon ' + _cssClass + '"';
        }
        icon += '></span>';
        if (_parent == "0") {
            return icon;
        } else {
            var $icon = $(icon);
            if (_position == "before") {
                _parent.prepend($icon);
            } else {
                _parent.append($icon);
            }
            return $icon
        }
    };

    GUI.image = function (_parent, _id, _url, _width, _height, _position) {
        var image = '<img id=' + _id + ' src="' + _url + '" />';
        var $image = $(image);
        if (_position == "before") {
            _parent.prepend($image);
        } else {
            _parent.append($image);
        }
        if (_width) {
            $image.width(_width);
        }
        if (_height) {
            $image.width(_height);
        }
        return $image;
    }


    GUI.mask = function (_id, _HTMLtext) {
        var dialog = '<div id=' + _id + ' ><span id="text-mask" style="position:relative;left:15px;bottom:10px;">' + _HTMLtext + '</span></div>';
        var $dialog = $(dialog);
        if ($('body').find('#' + _id).length) {
            $('body').append($dialog);
        } else {
            $('body').append($dialog);
        }
        $("#" + _id).dialog({
            dialogClass: 'alert',
            resizable: false,
            height: '105',
            width: '250',
            modal: true,
            position :['middle','middle'],
        });
        $(".ui-dialog-titlebar").hide();
         $("div.ui-dialog.ui-widget.ui-widget-content.ui-corner-all.alert.ui-draggable").css("top","50%");
       return $("#" + _id);
    };


    //checked
    GUI.messageDialog = function (_id, _title, _HTMLtext) {
        var dialog = '<div id=' + _id + ' title=' + _title + '>' + _HTMLtext + '</div>';

        var $dialog = $(dialog);
        $('body').append($dialog);
        $("#" + _id).dialog({
            resizable: false,
            height: 'auto',
            width: 'auto',
            modal: true
        });
        $("#" + _id).bind('dialogclose', function(event) {
             this.remove();
         });
        return $("#" + _id);
    };

    //checked
    GUI.confirmDialog = function (_id, _title, _text, _titleConfirmButton, _callback) {
        var dialog = '<div id=' + _id + ' title=' + _title + '><span style= "margin: 0 7px 20px 35px;" >' + _text + '</span></div>';
        var $dialog = $(dialog);
        $('body').append($dialog);
        $("#" + _id).dialog({
            resizable: false,
            height: 150,
            width: 300,
            modal: true,
            buttons: {
                Yes: function () {
                    _callback;
                    $(this).dialog("close");
                },
                No: function () {
                    $(this).dialog("close");
                    $(this).remove();
                }
            }
        })
        //GUI.addIcon($("#"+_id),"ui-icon-alert","margin: 0 7px 0 100px;","before");
        $('.ui-dialog :button').blur();
        return $("#" + _id);
    };

    // checked except mod vertical
    GUI.addStickyButton = function (_id, _items, _parent, _mode) {
        var size_list = _items.length;
        var stickyButton = '';
        if (size_list != 1) {
            stickyButton += '<div id="' + _id + '">';
        }
        for (var i = 0; i < size_list; i++) {
            stickyButton += '<input type="checkbox" id="' + _id + '-' + _items[i] + '"';
            stickyButton += '/><label class="' + _id + '-' + _items[i] + '" for="' + _id + '-' + _items[i] + '">' + _items[i] + '</label>';
        }
        if (size_list != 1) {
            stickyButton += '</div>';
        }
        if (_mode == "code") {
            return stickyButton;
        } else {
            var $stickyButton = $(stickyButton);
            _parent.append($stickyButton);
            if (size_list == 1) {
                $("#" + _id + '-' + _items[0]).button();
            } else if (_mode == "vertical") {
                $("#" + _id).buttonsetvertical();
            } else {
                $("#" + _id).buttonset();
            }
            return $stickyButton;
        }
    };

    //checked
    GUI.addCheckBox = function (_id, _text, _parent) {
        var checkBox = '<li id="' + _id + '" style="list-style:none;"><input type="checkbox"';
        checkBox += '><span>' + _text + '</span></li>';
        if (_parent) {
            var $checkBox = $(checkBox);
            _parent.append($checkBox);
            return $checkBox;
        } else {
            return checkBox;
        }
    };

    // checked except mod vertical	
    GUI.addStickyList = function (_id, _items, _parent, _mode) {
        var size_list = _items.length;
        var stickyList = '<form><div id="' + _id + '" >';
        for (i = 0; i < size_list; i++) {
            var tmp = '';
            if (i == _mode) {
                tmp = 'checked="checked"';
            }
            stickyList += '<input type="radio" id="' + _id + '-' + i + '" name="radio" ' + tmp + ' /><label for="' + _id + '-' + i + '">' + _items[i] + '</label>';
        }
        stickyList += '</div></form>';
        if (_mode = "code") {
            return stickyList;
        } else {
            var $stickyList = $(stickyList);
            _parent.append($stickyList);
            if (_mode == "vertical") {
                $("#" + _id).buttonsetv();
            } else {
                $("#" + _id).buttonset();
            }
            return $stickyList;
        }
    };

    //checked
    GUI.addRadioList = function (_id, _items, _parent) {
        var size_list = _items.length;
        var radioList = '<li id=' + _id + '>';
        for (var i = 0; i < size_list; i++) {
            var tmp = "";
            if (!i) {
                tmp = "checked";
            }
            radioList += '<input type="radio" name=' + _id + ' value=' + _items[i] + ' ' + tmp + '> ' + _items[i] + '<br>';
        }
        radioList += '</li>';
        if (_parent) {
            var $radioList = $(radioList);
            _parent.append($radioList);
            return $radioList;
        } else {
            return radioList;
        }
    };

    //checked
    GUI.addMenu = function (_item, _link, _position, _id) {
        if (!_link || !_position) return console.error("function:menuAddElement: miss argument");
        if (_item.length != _item.length) return console.error("function:menu invalid arguments, it must have same length");
        else {
            var menu = '<ul id="' + _id + '" class="z' + _position + '">';
            var size_menu = _item.length;
            if (_position != 0) {
                $('li#z' + _position).addClass("has-sub");
            }
            for (var i = 1; i < (size_menu + 1); i++) {
                if (_link[i - 1] != 0) {
                    if (_link[i - 1] == "checkbox") {
                        menu += GUI.addCheckBox('z' + _position + '_' + i, _item[i - 1]);
                    } else if (_link[i - 1] == "radioList") {
                        menu += GUI.addRadioList('z' + _position + '_' + i, _item[i - 1]);
                    } else {
                        menu += '<li id="z' + _position + '_' + i + '">' + '<a href="' + _link[i - 1] + '"><span>' + _item[i - 1] + '</span></a></li>';
                    }
                } else {
                    menu += '<li id ="z' + _position + '_' + i + '">' + '<a href="#"><span>' + _item[i - 1] + '</span></a></li>';
                }
            }
            menu += '</ul>';
            var $menu = $(menu);
            if (_position == '0') {
                $('div.ui-layout-north').append($menu); /*$("#"+_id+">li").css("border-right","30px dotted white")*/
            } else {
                $('li#z' + _position).append($menu);
            }
            return $menu;
        }
    };

    //not udpdated,Broken
    GUI.addElementMenu = function (_item, _link, _position) {
        if (!_link || !_position || !_item) return console.error("function:menuAddElement: miss argument");
        if (_item.length != _item.length) return console.error("function:menu invalid arguments, it must have same length");
        else {
            var _parent = $('div.ui-layout-north');
            if (_position != 0) {
                var nb = ($('li#z' + _position).children().size()) + 1;
            } else {
                var nb = ($('ul.' + _position).children().size()) + 1;;
            }
            var size_menu = _item.length;
            var menu = '<!---->';
            for (i = 1; i < (size_menu + 1); i++) {
                if (_link[i - 1] != 0) {
                    menu += '<li id=';
                    menu += '\'' + _position + '_' + nb + '\'>' + '<a href="' + _link[i - 1] + '">';
                    nb++;
                } else {
                    menu += '<li id =\'' + _position + '_' + nb + '\'>' + '<a href="#">';
                    nb++;
                }
                menu += '<span>' + _item[i - 1] + '</span></a></li>';
            }
            var $menu = $(menu);
            $('ul.' + _position).append($menu);
            return $menu;
        }
    };



    GUI.toolBar = function (_id, _parent, _icones, _links, _position) {
        var tomp = '<div id="icon-' + _id + '" class="settings-button" style="width:20px;height:20px;border-radius: 1px;">' + GUI.addIcon("0", "ui-icon-gear") + '</div>';
        var $tomp = $(tomp);
        _parent.append($tomp);
        var size_toolbar = _icones.length;
        var toolbar = '<div id="' + _id + '" class="toolbar-icons" style="display:none;">';
        for (var i = 0; i < size_toolbar; i++) {
            toolbar += '<a onclick="' + _links[i] + '"><i class="' + _icones[i] + '"></i></a>';
        }
        toolbar += '</div>';
        var $toolbar = $(toolbar);
        _parent.append($toolbar);
        var result = $tomp.toolbar({
            content: '#' + _id,
            position: _position,
            hideOnClick: true
        });
        $tomp.onmouseover = "this.style.cursor='pointer'";
        //$("div.tool-items a:first-child").remove();
        return result;
    };
    //----------------------------------------------------------------------------------------------------------------------------------------

    GUI.window = function (_txt, _pane, _float) {

        if (_pane)
            $parent = $(".ui-layout-" + _pane);

        // TODO - rename fragment-1 to something more useful
        var code =
            '<ul>' +
            '	<li><a href="#tabs-' + _pane + '-1" >' + _txt + '</a></li>' +
            '</ul>' +

        '<div class="ui-layout-content ui-widget-content" >' +
            '	<div id="tabs-' + _pane + '-1" class=" ui-widget-content" ></div>' +
            '</div>';
        var $window;

        if (!_pane || _float) {
            code = '<div class="ui-tabs ui-widget">' + code + '</div>';

            $window = $(code);
            $window.tabs();
            $('body').append($window);


            if (_float)
                $window.resizable({
                    helper: "ui-resizable-helper"
                })
                    .draggable();
        } else {
            $window = $(code);
            $parent.append($window);
            $parent.tabs()
            // allow tabs to be moved left/right
            .find(".ui-tabs-nav").sortable({
                axis: 'x',
                zIndex: 2
            });

        }

        //$window.uniqueId();
        //var id=$window.attr('id');

        return $window.find('.ui-widget-content');
    };


    // does not work without this
    GUI.resize = function (pane, $pane, paneState, paneOptions) {
        $pane.find('.ui-resize-me').resize();
    };
    
    //----------------------------------------------------------------------------------------------------------------------------------------
    GUI.searchClickId = function(event,element){
            var idSearch = event.target.id;
            var check = true; 
            var count = 0;
            var condition = ''
            while(check){
                count++;
                var search = $('#'+idSearch).attr('class');
                    if(search){
                        if(search.match("ui-layout-center")||search.match("ui-layout-south")||search.match("ui-layout-north")||search.match("ui-layout-est")||search.match("ui-layout-west")){
                            check=false;
                            break;}}
                    var $parent = $('#'+idSearch).parent(); 
                    idSearch = $parent.attr('id');
                    if(count==20){throw "didn't find any layouts"; return false;}
                    }
            element = $('#'+idSearch);
            var parentOffset = element.offset(); 
            var relX = event.pageX - parentOffset.left;
            var relY = event.pageY - parentOffset.top;
            var percentageY = Math.round(relY*(100/$(element).height()));
            var percentageX = Math.round(relX*(100/$(element).width()));
            return {id : idSearch, percentagex : percentageX, percentagey : percentageY};
            }



   GUI.Layout = function(id,position) {
    
        function Frame(id,position){
            this.id=id;
            this.position=position; 
            this.parent= {};
            this.jqueryObject = $([]);
            this.jqueryObjectPanes = $([]);
            this.jqueryObjectNorth = $([]);
            this.jqueryObjectSouth = $([]);
            this.jqueryObjectWest = $([]);
            this.jqueryObjectEst = $([]);
            this.jqueryObjectCenter = $([]);
            this.north = {};
            this.south = {};
            this.west = {};
            this.est = {};
            this.center = {};
            this.html = ''; 
            this.pane = [0,0,0,0,0];
            this.pane.size = 0;
            GUI.bufferLayout.push(this);
            
            this.reset = function(){
                this.id='';
                this.position=0; 
                this.idWrap = '';
                this.parent= {};
                this.child = {};
                this.jqueryObject = $([]);
                this.html = '';  
                this.north = {};
                this.south = {};
                this.west = {};
                this.est = {};
                this.center = {};
                this.pane = [0,0,0,0,0];
                this.pane.size = 0;
            }
                
            //check layout compatibility, if there are enough components to create a layout
            this.checkIn = function() {
                var checkNorth = jQuery.isEmptyObject(this.north) ? false : true;
                if(checkNorth&&! this.pane[0]){this.pane[0]="1";this.pane.size++;}
                var checkSouth = jQuery.isEmptyObject(this.south) ? false : true;
                if(checkSouth&&!this.pane[1]){this.pane[1]="1";this.pane.size++;}
                var checkEst = jQuery.isEmptyObject(this.est) ? false : true;
                if(checkEst&&!this.pane[2]){this.pane[2]="1";this.pane.size++;}
                var checkWest = jQuery.isEmptyObject(this.west) ? false : true;
                if(checkWest&&!this.pane[3]){this.pane[3]="1";this.pane.size++;}
                var checkCenter = jQuery.isEmptyObject(this.center) ? false : true;
                if(checkCenter&&!this.pane[4]){this.pane[4]="1";this.pane.size++;}
                var checkEmpty = jQuery.isEmptyObject(this.north)&&jQuery.isEmptyObject(this.south)&&jQuery.isEmptyObject(this.est)&&jQuery.isEmptyObject(this.west)&&jQuery.isEmptyObject(this.center) ? true : false;
                if((checkEmpty)||(this.pane.size<2)){return false;}
                else{return true;}
            }
            
            this.checkOut = function(){
                var txt = "Layout{"+this.id +"} which has "+this.pane.size+" panes. North:"+this.pane[0]+" South:"+this.pane[1]+" Est:"+this.pane[2]+" West:"+this.pane[3]+" Center:"+this.pane[4];
                return txt;
            }
            
            this.paneInfo = function(nb){
                if((!nb)&&((this.pane[nb]))){return {name:'north',link:this.north,object:this.jqueryObjectNorth};}
                if((nb==1)&&(this.pane[nb])){return {name:"south",link:this.south,object:this.jqueryObjectSouth};}
                if((nb==2)&&(this.pane[nb])){return {name:'est',link:this.est,object:this.jqueryObjectEst};}
                if((nb==3)&&(this.pane[nb])){return {name:'west',link:this.west,object:this.jqueryObjectWest};}
                if((nb==4)&&(this.pane[nb])){return {name:'center',link:this.center,object:this.jqueryObjectCenter};}
                else{return false;}
            }
            
            //Creation Layout
            this.create = function(){
                if(this.checkIn()){ 
                    console.log("creating... "+this.checkOut());
                    var layoutBuffer={};  
                    layoutBuffer['togglerLength_open']=0;
                    var selector  = ""; 
                    for(var i=0;i<this.pane.length;i++){ 
                        var info = this.paneInfo(i);
                        if(info){ 
                            if(selector){selector += ',';}
                            this.html += '<div id="'+this.id+'-'+info.name+'" class="ui-layout-'+ info.name +'"></div>';}
                            layoutBuffer[info.name] = info.link;} 
                    if(this.position==1){selector="#"+this.id+"-west,#"+this.id+"-center";}
                    this.jqueryObject=$(this.html);
                    if(this.position==1){this.parent = $('body');} 
                    this.parent.append(this.jqueryObject);
                    this.jqueryObject = this.parent.layout(layoutBuffer);
                    this.jqueryObjectPanes = $(selector);
                    this.jqueryObjectWest = $('#'+this.id+'-west');
                    this.jqueryObjectNorth = $('#'+this.id+'-north');
                    this.jqueryObjectSouth = $('#'+this.id+'-south');
                    this.jqueryObjectEst = $('#'+this.id+'-est');
                    this.jqueryObjectCenter = $('#'+this.id+'-center');
                    }
                else{console.error("Can't create "+this.checkOut());}}
            
            this.randomColor= function(){
                for(var i=0;i<this.pane.length;i++){
                    if(this.paneInfo(i)){
                    var color ='#'+Math.floor(Math.random()*16777215).toString(16); //paul irish 
                    $('#'+this.id+'-'+this.paneInfo(i).name).css('background',color);}
                    }}



            this.wrap = function(id,idSearch){
                var selector = $('#'+idSearch);
                if(selector.html()){
                    selector.children().wrapAll('<div id="'+id+'-center" class="ui-layout-center" style="height:100%;width:100%"></div>');
                }
                else{selector.append('<div id="'+id+'-center" class="ui-layout-center" style="height:100%;width:100%"></div>');}
            }
            
                
            this.cutH = function(id,position,idSearch){
                var positionOrigin = position;
                this.wrap(id,idSearch);
                var tmp = new Frame(id,this.position+1); 
                tmp.north = { size: position+"%" };
                position = 100 - position; 
                tmp.center = { size: position+"%" };
                tmp.parent=$('#'+idSearch); 
                tmp.create(); 
                tmp.randomColor();
                tmp.parent = this;
               // if(idSearch=="mainLayout-west"){tmp.cutH(GUI.uniqueId(),positionOrigin,tmp.id+"-center");}
           }
            
            this.cutV = function(id,position,idSearch){
                var positionOrigin = position;
                this.wrap(id,idSearch);
                var tmp = new Frame(id,this.position+1); 
                tmp.west = { size: position+"%" };
                position = 100 - position; 
                tmp.center = { size: position+"%" };
                tmp.parent=$('#'+idSearch); 
                tmp.create(); 
                tmp.randomColor();
                tmp.parent = this;
               // if(idSearch=="mainLayout-west"){tmp.cutV(GUI.uniqueId(),positionOrigin,tmp.id+"-center");}
            } 
            }

        
        var obj = new Frame(id,1); 
        obj.south = { closable: false,
                resizable: false,
                slidable: false,
                spacing_open: 0,
                spacing_closed: 0,};
        obj.center = {
                onresize: GUI.resize,
            };
        obj.north = {
                closable: false,
                resizable: false,
                slidable: false,
                spacing_open: 0,
                spacing_closed: 0,
            }; 
        obj.south = {
                resizable: true,
                closable: false,
                slidable: false,
                spacing_open: 0,
                spacing_closed: 0,
                resizerCursor: "move",
                onresize: GUI.resize,
            };
        obj.west = {
                size: "78%",
                resizerCursor: "move",
                onresize: GUI.resize,
            };
        obj.create();
        obj.jqueryObject.options.west.minSize = '10%';
        obj.jqueryObject.options.west.maxSize = '90%';
        obj.jqueryObject.allowOverflow("north");
        obj.jqueryObject.allowOverflow("south");   
        obj.jqueryObjectWest.append("<div id='support-layout' class='ui-layout-center' style='height:100%;width:100%'></div>");
        return obj;
   }
    
    

}).call(this);