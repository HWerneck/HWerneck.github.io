/*document.addEventListener("ClickOutsideProjectsMenu",
    function (event) {
        event.
    }
);*/

function displayProjectsMenu() {
    var menu = document.querySelector(".dropdown-menu");
    menu.setAttribute("style", "display: block;");
    return;
}

/*function displayProjectsMenu() {
    var menu = document.querySelector(".dropdown-menu");
    var menu_style = menu.getAttribute("style");
    if (menu_style.includes("display: block;") || (menu_style.includes("display:block;")) {
        menu.setAttribute("style", "display: none;");
    }
    else {
        menu.setAttribute("style", "display: block;");
    }
    return;
}*/