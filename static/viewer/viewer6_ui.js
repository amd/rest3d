/*
The MIT License (MIT)

Copyright (c) 2013 Rémi Arnaud - Advanced Micro Devices, Inc.

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
THE SOFTWARE.*/
'use strict';
define(['viewer', 'gui', 'rest3d', 'q', 'collada', 'gltf', 'renderer', 'state', 'channel', 'rest3dToTree', 'script', 'scene', 'console'],
    function (viewer, gui, rest3d, Q, COLLADA, glTF, RENDERER, State, Channel, rest3dToTree, script_init, scene_init, console_init) {

        viewer.databases = {};
        viewer.idUser = "User_" + Math.floor(Math.random() * 100000000000000) + 1;
        viewer.INIT = function () {
            "use strict"; //
            var mask;
            var win = $('');
            if (!window.performance || !window.performance.now) {
                window.performance = window.performance || {};
                window.performance.now = $.now
            };
            var listThemes = ["black-tie", "blitzer", "cupertino", "dark-hive", "dot-luv", "eggplant", "excite-bike", "flick", "hot-sneaks", "humanity", "le-frog", "mint-choc", "overcast", "pepper-grinder", "redmond", "smoothness", "south-street", "start", "sunny", "swanky-purse", "trontastic", "ui-darkness", "ui-lightness", "vader"];
            var renderMenu = $('');
            var url = "http://www.google.com/custom?q=fl4re&btnG=Search";
            var bufferGen = $("");

            window.fl4reStatus = function (type, text) {
                $('#copyButton').remove();
                $('#iconStatus').remove();
                $('#defaultText').remove();
                $('#defaultTextBis').remove();
                if (type == 'CLEAR' || type == 'READY') {
                    GUI.label("defaultText", "ready", window.layout.jqueryObjectSouth);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-circle-check", "float:left;margin:3px;", "before").attr('id', 'iconStatus');
                }
                else if (type == 'BUSY') {
                    GUI.label("defaultText", text, window.layout.jqueryObjectSouth);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-clock", "float:left;margin:3px;", "before").attr('id', 'iconStatus');;
                }
                else if (type == 'UPLOAD') {
                    GUI.label("defaultText", "Uploading... ", window.layout.jqueryObjectSouth).append(text);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-info", "float:left;margin:3px;", "before").attr('id', 'iconStatus');
                }
                else if (type == 'ERROR') {
                    GUI.label("defaultText", text, window.layout.jqueryObjectSouth);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-circle-close", "float:left;margin:3px;", "before").attr('id', 'iconStatus');
                }
                else if (type == 'WARNING') {
                    GUI.label("defaultText", text, window.layout.jqueryObjectSouth);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-alert", "float:left;margin:3px;", "before").attr('id', 'iconStatus');
                }
                else {
                    var label = GUI.label("defaultTextBis", text, window.layout.jqueryObjectSouth);
                    GUI.addIcon(window.layout.jqueryObjectSouth, "ui-icon-info", "float:left;margin:3px;", "before").attr('id', 'iconStatus');
                    var clear = GUI.button("Clear", label, function () {
                        GUI.copyToClipboard(text.slice(15, -1));
                        GUI.addTooltip({
                            parent: $('this'),
                            content: "copy completed!"
                        });
                    });
                    clear.prop("id", "copyButton");
                    clear.html('');
                    GUI.addIcon(clear, "ui-icon-copy", "", "before");
                    GUI.addTooltip({
                        parent: clear,
                        content: "copy URL to clipboard"
                    });
                }
            };

            window.loadCss = function (item) {
                var link = "../gui/themes/" + item + "/jquery-ui.css";
                GUI.destroyCurrentCssTheme();
                GUI.loadCssFile(link);
                GUI.currentCssTheme = link;
                setTimeout(function () {
                    GUI.setColorJqueryTheme();
                    GUI.refreshJqueryTheme();
                }, 10);


            }

            function pleaseWait(_displayMode) {
                if (!mask) {
                    mask = $([]);
                }
                if (_displayMode == true) {
                    var deferred = Q.defer();
                    mask = GUI.mask("mask-loading", "Please wait ...", $("body"));
                    mask.css({"top":"40% !important;"});
                    GUI.image($('#mask-loading'), "img-loading", "../gui/images/loading.gif", 30, 30, "before");
                    return deferred;
                }
                else if (_displayMode == false) {
                    setTimeout(function () {
                        mask.remove();
                    }, 500);
                }
            };
            window.pleaseWait = pleaseWait;
            var layout = GUI.Layout("mainLayout", 1);
            window.layout = layout;


            function jobmanager() {
                $("#dialog").dialog("close");
                var gitHtml = $('<div id="dialog"></div>');
                gitPanel = $('body').append(gitHtml);
                $("#dialog").dialog({
                    width: 300,
                    height: 300,
                    position: ['middle', 90],
                    title: 'Job manager',
                    open: function (ev, ui) {
                        // $('#myIframe').attr('src', url);
                    },
                    close: function () {
                        gitHtml.remove();
                    },
                });
            }

            var titleLabel = GUI.label("auteur", "@ rest3d.org", layout.jqueryObjectSouth);
            titleLabel.click(function () {
                $("#dialog").dialog("close");
                var gitHtml = $('<div id="dialog"><iframe id="myIframe" src="" style="height:100% !important; width:100% !important; border:0px;"></iframe></div>');
                gitPanel = $('body').append(gitHtml);
                $("#dialog").dialog({
                    width: '600',
                    height: '500',
                    open: function (ev, ui) {
                        $('#myIframe').attr('src', "http://rest3d.wordpress.com/");
                    },
                    close: function () {
                        gitHtml.remove();
                    },
                });
                $("#dialog").css({
                    "padding": "0",
                })
            })
           

            ////////////
            //        GUI.button('rambler-min', accordion.collada, function () {

            window.callbackFullscreen = function () {
                if (screenfull.enabled) {
                    screenfull.request();
                }
                else {
                    console.error("browser doesn't allow the fullscreen mode")
                }
            };
            var $html = $('<a href="#" onClick="window.callbackFullscreen()"></a>')
            $('body').append()
            $html.hide();

            var menu = GUI.menu({
                id: "menu",
                parent: layout.jqueryObjectNorth,
                item: [{
                    text: "Welcome guest!",
                    id: "welcomeMenu"
                }, {
                    text: "Management",
                    id: "management"
                }, {
                    text: "Settings",
                    id: "settings"
                }, {
                    text: "Themes UI",
                    id: "themes"
                }, {
                    text: "Support",
                    id: "support"
                }, {
                    text: "b",
                    id: "fullscreen",
                    callback: function () {
                        $html.click();
                    },
                }]
            });

            GUI.mainMenu = menu;


            // --------------------------------------------------------- support and help ----------------------------------
            var settingMenu = GUI.menu({
                id: "management-menu",
                parent: menu.management,
                item: [{
                    id: "jobManage",
                    text: 'Convert jobs',
                    callback: function () {
                        jobmanager();
                    }
                }, {
                    type: "separator"
                }, {
                    id: "databases",
                    text: "Databases",
                    callback: function () {
                        welcomePanel();
                    }
                }]
            });


            //Setting menu
            var settingMenu = GUI.menu({
                id: "settings-menu",
                parent: menu.settings,
                item: [{
                    id: "contextLoss",
                    text: 'Context Loss',
                    callback: function () {
                        button1.click();
                    }
                }, {
                    type: "separator"
                }, {
                    id: "animationFrame",
                    type: "checkbox",
                    text: "Request Animation Frame",
                    isChecked: function () {
                        $("#fpsCounterSetting").removeClass("disable");
                        viewer.flagTick = true;
                        viewer.tick();
                    },
                    noChecked: function () {
                        $("#fpsCounterSetting").addClass("disable");
                        if ($('#fpsCounterSetting').find('input').prop("checked") == true) {
                            $('#fpsCounterSetting').find('input').attr("checked", false);
                            $("#fps").hide();
                        }
                        viewer.flagTick = false;
                    }
                }, {
                    id: "fpsCounterSetting",
                    type: "checkbox",
                    text: "Show fps counter",
                    isChecked: function () {
                        $("#fps").show();
                    },
                    noChecked: function () {
                        $("#fps").hide();
                    },
                }, ]
            });


            var supportMenu = GUI.menu({
                id: "support-menu",
                parent: menu.support,
                item: [{
                    id: "help-panel",
                    text: "Help",
                    callback: "window.onHelp()"
                }, {
                    id: "google",
                    text: "Google",
                    callback: "window.onGithub()"
                }]
            })

            var helpPanel = $([]);

            //help dialog
            window.onHelp = function () {}

            var gitPanel = $([]);
            window.onGithub = function () {
                $("#dialog").dialog("close");
                var gitHtml = $('<div id="dialog"><iframe id="myIframe" src="" style="height:100% !important; width:100% !important; border:0px;"></iframe></div>');
                gitPanel = $('body').append(gitHtml);
                $("#dialog").dialog({
                    width: 600,
                    height: 500,
                    open: function (ev, ui) {
                        $('#myIframe').attr('src', url);
                    },
                    close: function () {
                        gitHtml.remove();
                    },
                    //  close: function(){
                    //     gitHtml.empty();
                    // },
                });
                $("#dialog").css({
                    "padding": "0",
                })

            }

            //////////////
            // THEMES MENU

            var listCallback = [];
            var jsonTheme = {
                id: 'theme-menu',
                parent: menu.themes,
                item: []
            };
            for (var lgt = 0; lgt < listThemes.length; lgt++) {
                jsonTheme.item[lgt] = {
                    text: listThemes[lgt],
                    id: listThemes[lgt],
                    callback: function (theme) {
                        return function () {
                            pleaseWait(true);
                            window.loadCss(theme);
                            pleaseWait(false);
                        }
                    }(listThemes[lgt])

                };
            }
            var theme = GUI.menu(jsonTheme);
            theme.setAutoHeight();


            //------------------------------ all menu icons ---------------------------------------------------------------------------------------------
            GUI.image(menu.welcomeMenu.text, "img-settings", "../gui/images/home.png", 13, 13, "before");
            GUI.image(menu.settings.text, "img-settings", "../gui/images/icon-cog.png", 14, 14, "before");
            GUI.image(menu.support.text, "img-help", "../gui/images/menu-help.png", 13, 13, "before");
            GUI.image(menu.themes.text, "img-themes", "../gui/images/jquery.png", 13, 13, "before");
            GUI.image(menu.management.text, "img-themes", "../gui/images/console_debug.png", 13, 13, "before");
            GUI.addIcon(menu.fullscreen.text, "ui-icon-arrow-4-diag", "position: relative !important; right:10px !important;bottom: 10px !important;", "before");
            menu.fullscreen.moveToRightFromLeft();
            //menu.management.moveToRightFromLeft(40);
            menu.support.moveToRightFromLeft(40);
            menu.themes.moveToRightFromLeft(150);
            menu.settings.moveToRightFromLeft(275);

            //------------------------------//render menu == TABS ---------------------------------------------------------------------------------------------

            var renderMenu = GUI.tab({
                id: "renderMenus",
                parent: layout.jqueryObjectCenter,
                item: [
                    {
                        id:'render',
                        text:'  Test'
                    },
                    {
                        id: "scenes",
                        text: " Scenes",
                    },
                    {
                    id: "script",
                    text: "  Script"
                },
                    {
                    id: "consoletab",
                    text:"  Console"
                    },
                ]
            })

            var westTab = GUI.tab({
                id: "westTab",
                parent: layout.jqueryObjectWest,
                item: [
                       {
                        id: "viewer",
                        text: " Viewer",
                    },
                ]
            })
        
            westTab.sortable();
            renderMenu.sortable();
            renderMenu.tabManager();
            window.renderMenu = renderMenu;
            window.westTab = westTab;
            window.westTab.viewer.css({'height':'100%'});

            GUI.image(westTab.viewer.title, "img-render", "../gui/images/3d_model.png", 12, 14, "before");
            GUI.image(window.renderMenu.scenes.title, "img-render", "../gui/images/scene-root1.png", 12, 14, "before");
            GUI.image(window.renderMenu.consoletab.title, "img-render", "../gui/images/console1.png", 12, 14, "before");


            scene_init(window.renderMenu);
            script_init(window.renderMenu);
            console_init(window.renderMenu);
            window.renderMenu.refresh();
            // -------------------  -----------------------------------------------------------------------------------------------------------------

            var canvas = GUI.canvas(westTab.viewer);
            $("#tmp").css({'height':'100%'});
            westTab.refreshSize();
            //layout.resetOverflow('west');
            // setTimeout(function(){
            //     var max = $('#westTab_content').height();
            //     westTab.viewer.height(max+2);
            //     $('#tmp').height(max+2);
            // },500)
            // $(window).on('resize orientationChanged', function () {
            //      setTimeout(function(){
            //     var max = $('#westTab_content').height();
            //     westTab.viewer.height(max);
            //     $('#tmp').height(max);
            // },500)

            // });

            // initialize webGL
            viewer.channel = Channel.create(canvas, false); // true for debug context

            var button1 = GUI.button('Simulate Context Loss', renderMenu.render, function () {
                if ($(this).find('.ui-button-text').text() == "Simulate Context Loss") {
                    Channel.forceContextLoss();
                    $(this).find('.ui-button-text').text("Simulate Restore Context");
                }
                else {
                    Channel.forceContextRestore();
                    $(this).find('.ui-button-text').text("Simulate Context Loss");
                }
            });
            initCanvasUI();

            // var currentZoom =0;
            // var currentRotationX, currentRotationY = 0;

            function initCanvasUI() {
                var mouseDown = false;
                var lastX, lastY = 0;
                var downX, downY = 0;
                // attach mouse events to canvas

                function mouseDownHandler(ev) {
                    if (!mouseDown) {
                        mouseDown = true;
                        downX = ev.screenX;
                        downY = ev.screenY;
                    }
                    lastX = ev.screenX;
                    lastY = ev.screenY;

                    return true;
                }

                function mouseMoveHandler(ev) {
                    if (!mouseDown) return false;

                    viewer.currentZoom = 1;
                    var mdelta = ev.screenX - lastX;
                    lastX = ev.screenX;
                    viewer.currentRotationX -= mdelta / 2.5;


                    var mdelta = ev.screenY - lastY;
                    lastY = ev.screenY;
                    viewer.currentRotationY += mdelta / 2.5;


                    viewer.draw();
                    return true;
                }

                function mouseUpHandler(ev) {
                    mouseDown = false;
                    viewer.currentZoom = 1;
                    // pick
                    if ((downX === ev.screenX) && (downY === ev.screenY)) {

                        var x = ev.layerX / ev.currentTarget.clientWidth;
                        var y = 1.0 - (ev.layerY / ev.currentTarget.clientHeight);

                        var id = viewer.pick(x, y);

                        if (viewer.pickName[id] != "undefined" && viewer.pickName[id] != null) {
                            if (!viewer.channel.selected) viewer.channel.selected = {};
                            if (viewer.channel.selected[id]) {
                                delete viewer.channel.selected[id];
                                window.fl4reStatus("", "selected " + viewer.pickName[Object.keys(viewer.channel.selected)[0]]);
                            }
                            else {
                                var realId = viewer.pickName[id].split("#").pop();
                                realId = realId.split("[")[0];
                                // console.debug("#"+realId+'__'+id+' '+$("#"+realId+'__'+id).length);
                                // treeScene.Tree.jstree("select_node", "#" + realId + '__' + id);
                                viewer.channel.selected[id] = true;
                                window.fl4reStatus("", "selected " + viewer.pickName[id]);
                                //$('#Treedef').jstree('select_node',realId); (select node in scene tree)
                            }
                            
                        }
                        else {
                            window.fl4reStatus("READY");
                            // if (treeScene) {
                            //     treeScene.Tree.jstree("deselect_all");
                            // }
                            delete viewer.channel.selected;
                        }
                        viewer.draw();
                    }
                }

                function mouseWheelHandler(ev) {

                    var mdelta = ev.wheelDelta;
                    viewer.currentZoom = Math.exp(mdelta / 2500);

                    viewer.draw();
                    return true;
                }

                canvas.removeEventListener("mousedown", mouseDownHandler, false);
                canvas.removeEventListener("mousemove", mouseMoveHandler, false);
                canvas.removeEventListener("mouseup", mouseUpHandler, false);
                canvas.removeEventListener("mousewheel", mouseWheelHandler, false);
                canvas.addEventListener("mousedown", mouseDownHandler, false);
                canvas.addEventListener("mousemove", mouseMoveHandler, false);
                canvas.addEventListener("mouseup", mouseUpHandler, false);
                canvas.addEventListener("mousewheel", mouseWheelHandler, false);

                function touchHandler(event) {
                    var touches = event.changedTouches,
                        first = touches[0],
                        type = "";
                    switch (event.type) {
                    case "touchstart":
                        type = "mousedown";
                        break;
                    case "touchmove":
                        type = "mousemove";
                        break;
                    case "touchend":
                        type = "mouseup";
                        break;
                    default:
                        return;
                    }

                    //initMouseEvent(type, canBubble, cancelable, view, clickCount, 
                    //           screenX, screenY, clientX, clientY, ctrlKey, 
                    //           altKey, shiftKey, metaKey, button, relatedTarget);

                    var simulatedEvent = document.createEvent("MouseEvent");
                    simulatedEvent.initMouseEvent(type, true, true, window, 1,
                        first.screenX, first.screenY,
                        first.clientX, first.clientY, false,
                        false, false, false, 0 /*left*/ , null);

                    first.target.dispatchEvent(simulatedEvent);
                    event.preventDefault();
                }


                canvas.addEventListener("touchstart", touchHandler, true);
                canvas.addEventListener("touchmove", touchHandler, true);
                canvas.addEventListener("touchend", touchHandler, true);
                canvas.addEventListener("touchcancel", touchHandler, true);


                // redraw on resize

                $(canvas).resize(function (evt) {
                    viewer.draw();
                });
            };

            button1.hide();

            setTimeout(function () {
                layout.jqueryObject.sizePane("west", $(window).width() - 599);
            }, 1000);

            layout.jqueryObject.resizeAll();
            layout.jqueryObject.initContent("center");
            layout.jqueryObject.initContent("west");

            window.loadCss("excite-bike");

            // $("#mainLayout-west").append('<div id="colorSelector"  style="z-index:9999!important;"><div style="background-color: #0000ff"></div></div>');
            GUI.button("undefined", westTab.viewer).prop("id", "colorSelector");
            var tmpPicker;
            var flagColorPicker = 'show';
            $('#colorSelector div').css('backgroundColor', '#ffffff');

            $('#colorSelector').ColorPicker({
                color: '#ffffff',
                onShow: function (colpkr) {
                    $(".colorpicker").css("margin-left", "-320px");
                    $(colpkr).fadeIn(200);
                    tmpPicker = $(colpkr);
                    return false;
                },
                onHide: function (colpkr) {
                    $(colpkr).fadeOut(200);
                    tmpPicker = $(colpkr);
                    return false;
                },
                onSubmit: function (hsb, hex, rgb) {
                    $('#colorSelector div').css('backgroundColor', '#' + hex);
                    westTab.viewer.css('backgroundColor', '#' + hex);
                    westTab.viewer.css({
                        "background-image": "none"
                    });
                    tmpPicker.fadeOut(200);
                }
            });
            // $("#tmp, #colorSelector, .colorpicker").mouseover(function(){
            //     $('#colorSelector').show();
            // })
            //   $("#tmp, #colorSelector, .colorpicker").mouseleave(function(){
            //     $('#colorSelector').hide();
            // })


            // VIEWER TOOLS SETTING UP
            var htmlFps = '<div class="yasuo" style="z-index: 999 !important;"><a id="fps" class="ui-widget-content" style="float:right;" >? FPS</a></div>';
            westTab.viewer.append(htmlFps);
            var but1 = GUI.button('fullscreen', westTab.viewer, function () {
                if (screenfull.enabled) {
                    screenfull.request(westTab.viewer[0]);
                }
            }).prop("id", "fullCanvas");
            but1.html('');
            GUI.addIcon(but1, "ui-icon-arrow-4-diag", "", "before");
            GUI.addTooltip({
                parent: but1,
                content: "Fullscreen mode",
            });
            $('#colorSelector').html('');
            GUI.addIcon($('#colorSelector'), "ui-icon-key", "", "before");
            GUI.addTooltip({
                parent: $('#colorSelector'),
                content: "Set a color to the background",
            });
            $('#colorSelector').css({
                "background": "none !important;"
            })
            var inputImage = GUI.input({
                id: "loadImage",
                parent: window.renderMenu.script,
                hide: true,
                extension: "image/*",
                mode: "displayImage",
            });
            var button = GUI.button('', westTab.viewer, function () {
                inputImage.click();
            }).prop("id", "imageCanvas");
            button.html('');
            GUI.addIcon(button, "ui-icon-image", "", "before");
            GUI.addTooltip({
                parent: button,
                content: "Load an image as background",
            });
            window.rest3dToTree=rest3dToTree;
            //WELCOME PANEL, panel displayed when the user session is 'new'
            function welcomePanel() {
                $("#dialog").dialog("close");
                var gitHtml = $('<div id="dialog"><img src="../gui/images/loading.gif"/></div>');
                gitPanel = $('body').append(gitHtml);
                function header(html, parent, text, draggableFlag, visibleFlag, button) {
                    this.flag = true;
                    this.html = html;
                    this.text = text;
                    this.parent = parent;
                    this.draggableFlag = draggableFlag;
                    this.visibleFlag = visibleFlag;
                    this.button = button;
                    this.createDraggable = function (flag) {
                        this.draggableZone = $('<div class="draggableWelcome ui-widget-header" style="display:inline-block"></div>');
                        this.header = this.header.append(this.draggableZone);
                        this.draggableZone.width("100%").height(71);
                        if (this.text == "Description&Features") {
                            var plus = $('<div style="float:right;margin-top:12px;margin-right:10px;"></div>');
                            this.draggableZone.append(plus);
                            GUI.image(plus, "img-welcome", "../gui/images/green-plus.png", 70, 40, "before");
                            GUI.addTooltip({
                                parent: plus,
                                content: "Add your own features",
                            });
                            plus.click(function () {
                                var tmp = GUI.messageDialog("yes", "UpFeatures", "");
                                tmp.parent().width(400);
                                tmp.css({'text-align':'center'});
                                tmp.dialog('option', 'position', 'center');
                                var tmp1 = GUI.image(tmp, "img-welcome", '../gui/images/under_construction.gif', 100, 100);
                                tmp1.css({'position':'center'});
                            })
                        }
                    }
                    this.createHeader = function () {
                        this.header = $("<div></div>");
                        this.linkArea = $("<div style = 'display:inline;'></div>");
                        this.link = $("<a style='float:right;'  href='#'>+" + this.text + "</a>");
                        // else{this.button="";}
                        this.parent.append(this.header)
                        if (this.button) {
                            this.button = $("<a style='float:left;'  href='#'>" + this.button + "</a>");
                            this.linkArea.append(this.button)
                        }
                        this.parent.append(this.linkArea, '<br><hr>');
                        this.linkArea.append(this.link);
                        this.header.append(this.html, "<br></br>");
                        this.header.css({
                            "text-align": "justify",
                            "text-justify": "inter-word"
                        })
                        this.header.hide();
                    }
                    this.createButton = function (name, data) {
                        var button = GUI.button("", this.draggableZone)
                        if (name == "") {
                            // button.prop('disabled', true);
                            var link = "../gui/images/upload_d.png";
                            GUI.addTooltip({
                                parent: button,
                                content: "This is your own temporary repository hosted by our cloud and used for linking node/the database",
                            });
                        }
                        else {
                            var link = data[name].picture;
                            GUI.addTooltip({
                                parent: button,
                                content: data[name].description,
                            });
                            button.data(data[name]);
                        }
                        // button.width(55).height(66);

                        if (link == "../gui/images/exist.png") {
                            GUI.image(button, "img-welcome", link, 77, 57, "before");
                            // $("#img-welcome").width("57px")
                        }
                        else {
                            GUI.image(button, "img-welcome", link, 77, 57, "before");
                        }
                        // button.width(57).height(77);
                        // button.height(85.781)
                        return button;
                    }
                    this.change = function () {
                        if (this.flag == true) {
                            this.header.show();
                            if (this.button) this.button.show();
                            this.link[0].innerText = "-" + this.text;
                            this.flag = false;
                        }
                        else {
                            this.header.hide();
                            if (this.button) this.button.hide();
                            this.link[0].innerText = "+" + this.text;
                            this.flag = true;
                        }
                    }
                    this.addDraggable = function () {
                        this.createDraggable();
                    }
                    this.createHeader();
                    var stock = this;
                    this.link.on("click", function () {
                        stock.change();
                    });
                    if (this.draggableFlag) {
                        this.addDraggable();
                        if (this.visibleFlag) {
                            this.link.click();
                        }
                    }
                }

                var setDraggable = function (object, drop, zone) {
                    object.draggable({
                        revert: "invalid",
                        cancel: false
                    });
                    drop.droppable({
                        activeClass: "ui-state-default",
                        hoverClass: "ui-state-hover",
                        drop: function (event, ui) {
                            var position = drop.find("button").last().position();
                            if (position == undefined) {
                                position = {
                                    "top": 84.5,
                                    "left": -50
                                };
                            }
                            drop.append(ui.draggable);
                            ui.draggable.css({
                                position: "absolute",
                                top: position.top,
                                left: position.left + 63
                            });
                            var json = ui.draggable.data();
                            if (!viewer.databases.hasOwnProperty(json.name)) {
                                viewer.databases[json.name] = json
                            }
                            else {
                                delete viewer.databases[json.name];
                            }
                            // object.remove(); 
                            // object.draggable("destroy");
                            // drop.droppable("destroy");
                            setDraggable(ui.draggable, zone, drop);
                        }
                    });
                }

                var callback = function (data) {
                    gitHtml.find("img").replaceWith("");
                    var what = "<a>We are proposing a new version of our viewer elaborated with gui6. In addition of the basic functionnalites available in the last version, you are now able to display some model samples in COLLADA/glTF format, convert a COLLADA object to glTF and edit a scene. You can either explore some solutions listed below for editing models hosted by their databases, upload your models to your own project stocked in our cloud or just work statically.";
                    what = new header(what, gitHtml, "Description&Features", true, true);
                    var guest = "<a>This section is dedicated to users who wants to fastly use our application. The user session will be automatically removed once expired. Drag and drop the features you want to exploit and get started!</a>";
                    guest = new header(guest, gitHtml, "Guest account", true, true, "Get started!");
                    // guest.createButton("", data);
                    var login = "<a>OAuth authentification not yet implemented</a>";
                    login = new header(login, gitHtml, "Login via google", false, false);
                    var about = "<a>AMD is sponsoring the main open-source (MIT licensed) prototype. It is a turn-key rest3d server composed of a XML database, a nodejs rest server, and viewer/loader code for the client. Source code available at https://github.com/amd/rest3d </a>";
                    about = new header(about, gitHtml, "About", false, false);
                    for(var key in data){
                        if(key!=="tmp"){setDraggable(what.createButton(key, data), guest.draggableZone, what.draggableZone);}
                        else{guest.createButton(key, data);}
                    }
                    // if (data.hasOwnProperty("dvia")) {
                    //     // tmp.prop("disabled",true)
                    //     // tmp.find('button').css({"pointer-events":"none"})
                    //     setDraggable(what.createButton("dvia", data), guest.draggableZone, what.draggableZone);
                    // }
                    // if (data.hasOwnProperty("warehouse")) {
                    //     setDraggable(what.createButton("warehouse", data), guest.draggableZone, what.draggableZone);
                    // }
                    // if (data.hasOwnProperty("db")) {
                    //     setDraggable(what.createButton("db", data), guest.draggableZone, what.draggableZone);
                    // }
                    guest.button.css({'font-size':'12px'});
                    guest.button.css({'font-weight':'bold'});
                    guest.button.click(function () {
                        viewer.databases["tmp"]=data.tmp;
                        for(var key in viewer.databases){
                            var name = key;
                            window.renderMenu.addTab({
                                id: "tab_"+name,
                                text: "  " + name,
                            });
                            var rest3dToTree = new window.rest3dToTree(viewer.databases[key],window.renderMenu["tab_"+name]);
                            $("#dialog").dialog("close");
                        }

                        // rest3dToTree.
                        $("#dialog").dialog("close");
                        window.renderMenu = renderMenu;
                        // if (viewer.databases.hasOwnProperty("warehouse")) {
                        //     require(["database"], function (databaseTab) {
                        //         var tmp = new databaseTab(viewer.databases.warehouse);
                        //     });
                        // }
                        // if (viewer.databases.hasOwnProperty("3dvia")) {
                        //     require(["database"], function (databaseTab) {
                        //         var tmp = new databaseTab(viewer.databases["3dvia"]);
                        //     });
                        // }
                        // if (data.hasOwnProperty("db")) {
                        //     require(["database"], function (databaseTab) {
                        //         var tmp = new databaseTab(data.db);
                        //     });
                        // }
                    })
                }

                $("#dialog").dialog({
                    width: '500',
                    height: 'auto',
                    position: ['middle', 160],
                    title: 'Welcome to rest3d!',
                    modal: true,
                    create: function () {
                        $(this).css("maxHeight", 450);
                    },
                    open: function (ev, ui) {
                        rest3d.info(callback);
                    },
                    close: function () {
                        gitHtml.remove();
                    },
                });
                rest3d.info().then(callback);
            }
            //FPS counter

            viewer.fpsCounter = new viewer.FPSCounter();
            viewer.flagTick = false;
            viewer.fpsCounter.createElement(document.getElementById("fps"));
            viewer.fpsCounter.run();
            $("#fps").hide();
            viewer.tick();
            //set background color to green
            westTab.viewer.css('backgroundColor', '#b9f09e');
            //init bar status
            window.fl4reStatus("READY");
            //Init Tabs
            GUI.container.resizeAll();
            GUI.container.initContent("center");
            GUI.container.initContent("west");
            $("#defaultText").text($("#defaultText").text()+" | ");
            window.consoleStatus = $("<a href='#'></a>");
            $("#defaultText").append(window.consoleStatus);
            window.consoleStatus.click(function(){
                window.renderMenu.consoletab.focusTab();
            });
            var tmp = GUI.image(layout.jqueryObjectSouth, "img-settinfsfdsdsdsdsgs", "../gui/images/bundle_social.jpg", 100, 90);
            tmp.css({"float":"right"})
           
            //tick the server every minutes to keep alive the session 
            setInterval(function(){
                rest3d.info()
            },60*1000)

            //Linking UI to the rest3d API 
             // welcomePanel();
            rest3d.database({}, function(data){
                //  if (jQuery.isEmptyObject(data.children) && jQuery.isEmptyObject(data.assets)) { // If no activities on the tmp repository exist
                //     welcomePanel();
                // }
                // else { // else check databases enabled from the server and load their tabs
                    // tmp.infoServer(); // lunch request
                    rest3d.info(function(data){
                        for(var key in data){
                                var name = key;
                                window.renderMenu.addTab({
                                    id: "tab_"+name,
                                    text: "  " + name,
                                });
                                var rest3dToTree = new window.rest3dToTree(data[key],window.renderMenu["tab_"+name]);
                                //$("#dialog").dialog("close");
                            }
                        });
                    // setTimeout(function () {
                    //     tmp.tree.jstree('open_all');
                    // }, 1500); // open all nodes of the tab
                // }              
            }, "/info/tmp");

            window.westLayout = window.layout.cutH("westLayout",40,"mainLayout-west");
            window.westLayout.parent.find(".ui-layout-resizer").addClass("ui-widget-header");
            window.centerLayout = window.layout.cutH("centerLayout",30,"mainLayout-center");
            window.centerLayout.parent.find(".ui-layout-resizer").addClass("ui-widget-header");
            window.centerLayout.jqueryObject.hide("south");
            window.westLayout.jqueryObject.hide("south");
            window.notif = function (object) {
                win.parent().parent().find(".ui-icon-close").click();
                win.remove();
                GUI.notification({
                    title: object + " successfuly loaded",
                    text: "<div id='informationDisplay'></div>",
                    type: "notice",
                });
                win = $('#informationDisplay');
                GUI.label("zoom", "currentZoom is 1", win);
                win.append('</br>');
                GUI.label('rot', 'rotation goes here', win);
                win.append('</br>');
                GUI.label('loadtimer', 'loading time: 0', win);
                win.append('</br>');
                GUI.label('rdm1', 'Initializing canvas mouse events', win);
                win.append('</br>');
                GUI.label('rdm12', 'Use left mouse click to rotate', win);
                win.append('</br>');
                GUI.label('rdm13', 'Use mouse wheel to zoom', win);
            }
             function jumpLine() {
                renderMenu.render.append("<br></br>");
            }

            renderMenu.render.append("<h4>Test tab for displaying models</h4>");

            jumpLine();

            var accordion = GUI.accordion({
                id: "menu-render",
                parent: window.renderMenu.render,
                item: [{
                    id: "collada",
                    text: ""
                }, {
                    id: "gltf",
                    text: ""
                }, ]
            })

            accordion.autoScrollDown();
            GUI.image(accordion.collada.header, "img-settings", "../gui/images/collada.png", 40, 80, "before");
            GUI.image(accordion.gltf.header, "img-settissngs1", "../gui/images/glTF.png", 40, 35, "before");
            GUI.image(window.renderMenu.render.title, "img-render", "../gui/images/menu-render.png", 12, 14, "before");

            GUI.button('cat skinned (work in progress)', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/cat/cat-skin.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    }, function (error) {
                        // If there's an error or a non-200 status code, log the error.
                        console.error("cs " + error);
                    });
            }).width("90%");
            accordion.collada.append("<hr></hr>");


            GUI.button('cat skinned', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/cat/cat-skin.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");


            GUI.button('gradient', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/gradient/gradient.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");


            GUI.button('duck', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/duck/duck.dae"
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    }, function (error) {
                        // If there's an error or a non-200 status code, log the error.
                        pleaseWait(false);
                        console.error("cs " + error);
                    });
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('creature', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/Amahani/Amahani.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('skinned creature', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/Amahani/Amahani.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('duck', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/duck-glft/duck_triangulate.json"
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('rambler', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/rambler/Rambler.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })

            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('rambler-min', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/rambler/Rambler-min.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })

            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('rambler', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/rambler/Rambler.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('supermurdoch', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/SuperMurdoch/SuperMurdoch.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('supermurdoch', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/SuperMurdoch/SuperMurdoch.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('wine', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/wine/wine.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('plane', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/plane/seymourplane_angle_linear_animation.dae";
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('wine', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/wine/wine.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");


            GUI.button('uh-1n', accordion.collada, function () {
                pleaseWait(true);
                var url = "/models/uh-1n/uh-1n.dae"
                COLLADA.load(url, viewer.parse_dae).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.collada.append("<hr></hr>");

            GUI.button('uh-1n', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/uh-1n/uh-1n.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('cow', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/cow/cow.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");

            GUI.button('tim_camera_monster', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/tim_cymbalsMonster_chip/cymbalsMonster_chip_camera.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url.split('/').pop());
                        setTimeout(function(){$('#Treedef').jstree("refresh")},1000);
                    })
            }).width("90%");
            accordion.gltf.append("<hr></hr>");



            GUI.button('cat skinned + animation', accordion.gltf, function () {
                pleaseWait(true);
                var url = "/models/cat/20_cat_smooth_bake_channel.json";
                glTF.load(url, viewer.parse_gltf).then(
                    function (flag) {
                        pleaseWait(false);
                        window.notif(url);
                    })
            }).width("90%");
        }
        
        return viewer;
    });




