const divId = "hfCarouseItemDisplayDiv"
const innerId = "hfCarouseItemDisplayInnerDiv"
const countBtnId = "hfHabitCarouseItemDisplayCountBtn"
const imageId = "hfCarouseItemDisplayImage"

function hideButton() {
    // 获取目标节点
    var btn = document.getElementById(countBtnId);
    if (btn) {
        // 直接删除目标节点
        btn.remove();
    }
}

function showCounterButton(carouseItem, f) {
    var countButton = document.createElement("button");
    countButton.setAttribute("id", countBtnId);
    countButton.style.alignItems = "center";
    countButton.style.justifyContent = "center";
    countButton.style.position = "absolute"; // 使关闭按钮的位置相对于 div 定位
    countButton.style.top = "5px"; // 使关闭按钮距离 div 顶部 5px
    countButton.style.right = "45px"; // 使关闭按钮距离 div 右侧 5px
    countButton.style.width = "30px"; // 设置按钮的宽度
    countButton.style.height = "30px"; // 设置按钮的高度
    countButton.style.padding = "0"; // 设置按钮的内边距
    countButton.style.border = "none"; // 设置按钮的边框
    countButton.style.backgroundColor = "#cccccc"; // 设置按钮的背景颜色
    countButton.style.cursor = "pointer"; // 设置按钮的鼠标指针样式
    countButton.style.borderRadius = "50%"; // 设置按钮的圆角
    countButton.style.outline = "none";
    countButton.onclick = f

    var img = document.createElement("img");
    const iconPath = chrome.runtime.getURL("images/record.png");
    img.style.opacity = "1"; // 设置图片的透明度
    img.style.width = "70%"; // 设置 img 的宽度为 div 的70%
    img.style.height = "70%"; // 设置 img 的高度为 div 的70%
    img.style.borderRadius = "2px"; // 设置图片的圆角
    img.style.objectFit = "scale-down"; // 使图片随 div 的大小变化
    img.src = iconPath

    countButton.appendChild(img)

    var innerDiv = document.getElementById(innerId)
    innerDiv.appendChild(countButton)
}

function setImageSrc(url) {
    var newImg = document.getElementById(imageId)
    newImg.src = url;
}

function checkDivExist() {
    var displayDiv = document.getElementById(divId)
    if (displayDiv) {
        return true;
    }
    return false;
}

function showElement() {
    var displayDiv = document.getElementById(divId)
    displayDiv.style.display = "flex"
}

function insertElement() {
    // 修改页面背景色
    // document.body.style.backgroundColor = "grey";

    // 在页面中插入一个新元素
    const newDiv = document.createElement("div");
    newDiv.setAttribute("id", divId);
    newDiv.style.position = "fixed";
    // TODO：根据显示位置设置参数
    newDiv.style.bottom = "10px";
    newDiv.style.right = "10px";
    newDiv.style.backgroundColor = "rgba(244, 244, 244, 0.2)";
    newDiv.style.color = "white";
    newDiv.style.padding = "10px";
    newDiv.style.borderRadius = "5px"; // 设置图片的圆角
    newDiv.style.opacity = "0.9"; // 设置透明度
    newDiv.style.resize = "both";
    newDiv.style.overflow = "auto"; 
    newDiv.style.display = "flex"; // 以便于在 div 内部定位元素
    newDiv.style.flexDirection = "column"; // 以便于在 div 内部垂直排列元素
    newDiv.style.width = "417px";
    newDiv.style.height = "314px";
    newDiv.style.zIndex = "999999999"; // 设置 div 的 z-index
    const angle = Math.floor(417 / 360 * 100);
    const gradient = `linear-gradient(${ angle }deg, rgba(0,143,104,1) 50%, rgba(250,224,66,1) 50%)`;
    newDiv.style.background = gradient;

    // 创建一个内部的 div 元素
    const innerDiv = document.createElement("div");
    innerDiv.setAttribute("id", innerId)
    innerDiv.style.position = "relative"; // 以便于在 div 内部定位关闭按钮
    innerDiv.style.opacity = "1"; // 设置图片的透明度，为不透明

    // 创建一个新的 img 元素
    const newImg = document.createElement("img");
    newImg.setAttribute("id", imageId);
    newImg.src = "https://raw.githubusercontent.com/guowei1651/Habit-Formation-Auditorium/refs/heads/main/test/73b8a0bcf8c74d86a4c51bcff5814ebf.jpeg"; // 设置 img 的图片地址
    newImg.style.opacity = "1"; // 设置图片的透明度
    newImg.style.width = "100%"; // 设置 img 的宽度为 div 的100%
    newImg.style.height = "100%"; // 设置 img 的高度为 div 的100%
    newImg.style.borderRadius = "5px"; // 设置图片的圆角
    newImg.style.objectFit = "scale-down"; // 使图片随 div 的大小变化

    // 创建一个新的关闭按钮
    const closeButton = document.createElement("button");
    // closeButton.textContent = "X"; // 设置按钮的文本为 "×"
    closeButton.style.position = "absolute"; // 使关闭按钮的位置相对于 div 定位
    closeButton.style.top = "5px"; // 使关闭按钮距离 div 顶部 5px
    closeButton.style.right = "5px"; // 使关闭按钮距离 div 右侧 5px
    closeButton.style.width = "30px"; // 设置按钮的宽度
    closeButton.style.height = "30px"; // 设置按钮的高度
    closeButton.style.padding = "0"; // 设置按钮的内边距
    closeButton.style.border = "none"; // 设置按钮的边框
    closeButton.style.backgroundColor = "#cccccc"; // 设置按钮的背景颜色
    closeButton.style.color = "black"; // 设置按钮的文字颜色
    closeButton.style.cursor = "pointer"; // 设置按钮的鼠标指针样式
    closeButton.style.borderRadius = "50%"; // 设置按钮的圆角
    closeButton.onclick = function() { 
        newDiv.style.display = "none"; 
    };

    var btnImg = document.createElement("img");
    const iconPath = chrome.runtime.getURL("images/close.png");
    btnImg.style.opacity = "1"; // 设置图片的透明度
    btnImg.style.width = "70%"; // 设置 img 的宽度为 div 的70%
    btnImg.style.height = "70%"; // 设置 img 的高度为 div 的70%
    btnImg.style.borderRadius = "2px"; // 设置图片的圆角
    btnImg.style.objectFit = "scale-down"; // 使图片随 div 的大小变化
    btnImg.src = iconPath
    closeButton.appendChild(btnImg)

    // 将 img 元素和关闭按钮添加到 div 元素中
    innerDiv.appendChild(newImg);
    innerDiv.appendChild(closeButton);

    newDiv.appendChild(innerDiv);
    document.body.appendChild(newDiv);
}