var applications

var bg_color = 'rgb(47, 19, 16)'
var download_color = 'rgb(103, 14, 29)'
var installation_color = 'rgb(207, 28, 59)'

function updateList(selectId) {
    var $this = $(selectId), numberOfOptions = $(selectId).children('option').length;

    var $styledSelect = $this.next('div.select-styled');
    $styledSelect.children(".select-options").remove()

    $styledSelect.text($this.children('option').eq(0).text());

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
        $this.val($(this).attr('rel'));
        $list.hide();
        $this.selectedIndex=$this.val();
        if ($this.attr("id")==="application") {
            updateVersions()
        }
    });
}

function updateVersions() {
    selection = $("#application").val()
    if (applications) {
        $versionSelect = $("#version")
        $versionSelect.empty()
        $.each(applications[selection].versions, function(index, version) {
            var $option = $("<option></option>").attr("value", index).text(version)
            $versionSelect.append($option)
        })
        updateList("#version")
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

ipcRenderer.on('packet-from-console', function(event, arg) {
    if (arg.initialize) {
        applications = arg.initialize.applications

        var $appSelect = $("#application")

        $appSelect.empty()
        $.each(applications, function(i, app) {
            var $option = $("<option></option>").attr("value", i).text(app.name)
            $appSelect.append($option)
        })
        updateList("#application")
        updateVersions()
        $("#loading").hide()
        $("#main_form").show()
    }
    var dashValue = parseFloat($("#progress-bar").css('stroke-dasharray'))

    $install = $("#install")

    if (arg.download_progress) {
        updateInstallButton(false, "Downloading: <b>"+ arg.download_progress + "%</b>")
        $("#spinner").addClass("rotating")
        $("#progress-bg").css('stroke', bg_color)
        $("#progress-bar").css('stroke', download_color)
        $("#progress-bar").css('stroke-dashoffset', (dashValue - arg.download_progress/100 * dashValue))
        $("#error-message").hide()
    }
    if (arg.installation_progress) {
        updateInstallButton(false, "Installing: <b>"+ arg.installation_progress + "%</b>")
        $("#spinner").addClass("rotating")
        $("#progress-bg").css('stroke', download_color)
        $("#progress-bar").css('stroke', installation_color)
        $("#progress-bar").css('stroke-dashoffset', (dashValue - arg.installation_progress/100 * dashValue))
        $("#error-message").hide()
    }
    if (arg.installation_progress >= 100) {
        updateInstallButton(true, "Done!")
        $("#spinner").removeClass("rotating")
        $("#logo-area").css('background-image', 'url(img/luna_logo.svg)')
        $("#spinner").hide()
    }
    if (arg.error) {
        updateInstallButton(true, "Try again")
        $("#spinner").removeClass("rotating")
        $("#error-message").html("<b>Error</b>: " + arg.error).show()
    }
})

$("#application").change(updateVersions)


$('div.select-styled').click(function(e) {
    e.stopPropagation();
    $('div.select-styled.active').not(this).each(function(){
        console.log("hiding", $(this).attr('id'))
        $(this).removeClass('active').next('ul.select-options').hide();
    });
    $(this).toggleClass('active').next('ul.select-options').toggle();
})

$(document).click(function() {
    $('div.select-styled.active').each(function() {
        $(this).removeClass('active').next('ul.select-options').hide()
    })
})

$("#install").click(function() {
    if ($(this).html() === "Done!") {
        var window = remote.getCurrentWindow();
        window.close();
    } else {
        $("#logo-area").css('background-image', 'url(img/luna_logo_without_border.svg)')
        $("#spinner").show()

        var install = {
            "install": {
                "application": $("#application option:selected").text(),
                "version": $("#version option:selected").text()
            }
        }
        ipcRenderer.send("packet-to-console", install)
    }
})

ipcRenderer.send('ready')
