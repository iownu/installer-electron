var applications
var versionTypes
var extendedVersionTypes

var installation_began = false
var installation_complete = false
var close_on_install_button_click = false
var developer_builds_are_shown = false
var ask_for_email = true

function updateList(selectId) {
    var $this = $(selectId), numberOfOptions = $(selectId).children('option').length;

    var $styledSelect = $this.next('div.select-styled');
    var $arrow = $this.siblings('div.arrow')
    $styledSelect.next(".select-options").remove()
    $styledSelect.text($this.children('option').eq(0).text());

    $arrow.removeClass("up")
    if (numberOfOptions <= 1) {
        $arrow.addClass("hidden")
        $styledSelect.removeClass("clickable")
        // dont add select-options if there is only one option
    } else { //if (numberOfOptions > 1)s
        $arrow.removeClass("hidden")
        $styledSelect.addClass("clickable")

        var $list = $('<ul />', {
            'class': 'select-options'
        }).insertAfter($styledSelect);

        for (var i = 0; i < numberOfOptions; i++) {
            $('<li />', {
                text: $this.children('option').eq(i).text(),
                rel: $this.children('option').eq(i).val()
            }).appendTo($list);
        }

        var $listItems = $list.children('li');

        // items list is recreated each time
        $listItems.click(function(e) {
            e.stopPropagation();
            $styledSelect.text($(this).text()).removeClass('active');
            $arrow.removeClass('up');
            $this.val($(this).attr('rel'));
            $list.hide();
            $this.selectedIndex=$this.val();
            if ($this.attr("id")==="application") {
                updateVersions()
            }
        });
    }
}

function updateVersions() {
    selection = $("#application").val()
    if (applications && versionTypes) {
        $versionSelect = $("#version")
        $versionSelect.empty()
        $.each(versionTypes, function(type_index, version_type)
        {
            $.each(applications[selection].versions[version_type], function(index, version) {
                let value = type_index+"_"+index
                var $option = $("<option></option>").attr("value", value).text(version)
                $versionSelect.append($option)
            })
        })
        updateList("#version")
        addTypesOfVersionsHeadings()
    }
}

function addTypesOfVersionsHeadings() {
    var $list = $("#version").next('div.select-styled').next('ul.select-options').children('li')
    var last_type = -1
    $list.each(function(index, item) {
        let type = $(item).attr('rel').split("_")[0]

        if (type != last_type) {
            if (last_type !== -1) {
                $('<hr />').insertBefore(item)
            }
            $('<h2 />', { text: versionTypes[type] }).insertBefore(item)
            last_type = type
        }
    })
}

function showDeveloperBuilds() {
    if (!developer_builds_are_shown) {
        developer_builds_are_shown = true
        versionTypes = extendedVersionTypes
        updateVersions()
        var $message = $("#message")
        $message.html("Developer builds are shown").addClass("displayed")
        setTimeout(function() {
            if ($message.html() === "Developer builds are shown") {
                $message.removeClass("displayed")
            }
        }, 2000)
    }
}

function updateInstallButton(clickable, text)
{
    $install = $("#install")
    if (clickable)
        $install.addClass("clickable").removeClass("button-extended")
    else
        $install.removeClass("clickable").addClass("button-extended")
    $install.html(text)
}

function displayError(errorMsg)
{
    updateInstallButton(true, "Close")
    close_on_install_button_click = true
    $("#spinner").removeClass("rotating")
    $("#message").html("<span><p><b>Error</b>: " + errorMsg + "</p><p id=\"copy_error\">Copy details to clipboard</p></span>").addClass("displayed").addClass("error")
    $("#message").find("#copy_error").click(function() {
        clipboard.writeText(errorMsg)
    })
}

ipcRenderer.on('packet-from-console', function(event, arg) {
    if (arg.initialize) {
        applications = arg.initialize.applications
        versionTypes = arg.initialize.versionTypes.filter(function(version) { return !version.startsWith(".") })
        extendedVersionTypes = arg.initialize.versionTypes.map(function(version) { return version.startsWith(".") ? version.substr(1) : version })
        ask_for_email = arg.initialize.askEmail

        var $appSelect = $("#application")

        $appSelect.empty()
        $.each(applications, function(i, app) {
            var $option = $("<option></option>").attr("value", i).text(app.name)
            $appSelect.append($option)
        })
        updateList("#application")
        updateVersions()
        $("#loading").hide()
        if (ask_for_email) {
            $("#email_form").addClass("visible")
        } else {
            $("#main_form").addClass("visible")
        }
    }
    var dashValue = parseFloat($("#progress-bar").css('stroke-dasharray'))

    $install = $("#install")

    if (arg.download_progress) {
        updateInstallButton(false, "Downloading: <b>"+ Math.round(arg.download_progress*100) + "%</b>")
        $("#error-message").removeClass("displayed")
        lll.set_downloading($("#logo-area"), arg.download_progress)
    }
    if (arg.installation_progress) {
        updateInstallButton(false, "Installing: <b>"+ Math.round(arg.installation_progress*100) + "%</b>")
        $("#error-message").removeClass("displayed")
        lll.set_loading($("#logo-area"), arg.installation_progress)
    }
    if (arg.installation_progress >= 1.0) {
        updateInstallButton(true, "Done!")
        close_on_install_button_click = true
        installation_complete = true
        lll.set_complete($("#logo-area"))
    }
    if (arg.error) {
        displayError(arg.error)
    }
    if (arg.message) {
        $("#message").html("<span>" + arg.message + "</span>").addClass("displayed")
    }
})

ipcRenderer.on('console-closed', function(event, closeInfo) {
    if (closeInfo.stderr) {
        displayError(closeInfo.stderr)
    } else if (closeInfo.code != 0) {
        displayError("Installation process exits with code "+closeInfo.code)
    } else if (!installation_complete) {
        displayError("Installation process ended abruptly")
    }
})

$("#application").change(updateVersions)


$('div.select-styled').click(function(e) {
    e.stopPropagation();
    $('div.select-styled.active').not(this).each(function(){
        $(this).removeClass('active').next('ul.select-options').hide();
        $(this).next('div.arrow').removeClass('up');
    });
    if (!installation_began) {
        $(this).toggleClass('active').next('ul.select-options').toggle();
        $(this).siblings('div.arrow').toggleClass('up');
    }
})

$(document).click(function() {
    $('div.select-styled.active').each(function() {
        $(this).removeClass('active').next('ul.select-options').hide()
        $(this).siblings('div.arrow').toggleClass('up');
    })
})

$(document).keypress(function(event)
{
    if (event.which === 4) { // Ctrl+D
        showDeveloperBuilds()
    }
})

// See http://rosskendall.com/blog/web/javascript-function-to-check-an-email-address-conforms-to-rfc822
function isEmail(email){
    return /^([^\x00-\x20\x22\x28\x29\x2c\x2e\x3a-\x3c\x3e\x40\x5b-\x5d\x7f-\xff]+|\x22([^\x0d\x22\x5c\x80-\xff]|\x5c[\x00-\x7f])*\x22)(\x2e([^\x00-\x20\x22\x28\x29\x2c\x2e\x3a-\x3c\x3e\x40\x5b-\x5d\x7f-\xff]+|\x22([^\x0d\x22\x5c\x80-\xff]|\x5c[\x00-\x7f])*\x22))*\x40([^\x00-\x20\x22\x28\x29\x2c\x2e\x3a-\x3c\x3e\x40\x5b-\x5d\x7f-\xff]+|\x5b([^\x0d\x5b-\x5d\x80-\xff]|\x5c[\x00-\x7f])*\x5d)(\x2e([^\x00-\x20\x22\x28\x29\x2c\x2e\x3a-\x3c\x3e\x40\x5b-\x5d\x7f-\xff]+|\x5b([^\x0d\x5b-\x5d\x80-\xff]|\x5c[\x00-\x7f])*\x5d))*$/.test( email );
}

function emailAddressIsValid() {
    return isEmail($("#email").val())
}

$("#next").click(function() {
    if (emailAddressIsValid()) {
        $("#email_form").removeClass("visible")
        $("#main_form").addClass("visible")
    }
})

$("#email").on('input', function() {
    if (emailAddressIsValid()) {
        $("#next").removeClass("disabled").addClass("clickable")
    } else {
        $("#next").addClass("disabled").removeClass("clickable")
    }
})

$("#email").keypress(function(event)
{
    if (event.which === 13 && emailAddressIsValid()) {
        $("#next").click()
    }
})

$("#install").click(function() {
    if (close_on_install_button_click) {
        var window = remote.getCurrentWindow();
        window.close();
    } else if (!installation_began) {
        $('div.select-styled.active').each(function(){
            $(this).removeClass('active').next('ul.select-options').hide();
            $(this).next('div.arrow').toggleClass('up');
        });
        $('div.select-styled.clickable').removeClass('clickable')
        $('div.select-styled').siblings('div.arrow').css('opacity', '0.0')
        $('#agree-box').prop('disabled', true).next('label').removeClass('clickable');

        installation_began = true;

        var install = {
            "install": {
                "application": $("#application option:selected").text(),
                "version": $("#version option:selected").text(),
            }
        }
        if (ask_for_email) {
            install["install"]["email"] = $("#email").val()
        }

        ipcRenderer.send("packet-to-console", install)
        installation_began = true;
    }
})

$("#close").click(function() {
    var window = remote.getCurrentWindow();
    window.close();
})

$('a[href^="http"]').on('click', function(event) {
    event.preventDefault();
    shell.openExternal(this.href);
});

ipcRenderer.send('ready')
