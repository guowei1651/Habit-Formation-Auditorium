// https://developer.mozilla.org/zh-CN/docs/Mozilla/Add-ons/WebExtensions
console.log("content-script.js start");

// 测试 browser 对象是否可用
if (typeof browser !== "undefined") {
  console.log("Browser API is available in content script.");
} else {
  console.error("Browser API is undefined in content script.");
}

// 修改页面背景色
// document.body.style.backgroundColor = "grey";

// 在页面中插入一个新元素
const newDiv = document.createElement("div");
// newDiv.textContent = "这是一个由插件插入的内容";
newDiv.style.position = "fixed";
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

// 创建一个内部的 div 元素
const innerDiv = document.createElement("div");
innerDiv.style.position = "relative"; // 以便于在 div 内部定位关闭按钮
innerDiv.style.opacity = "1"; // 设置图片的透明度，为不透明

// 创建一个新的 img 元素
const newImg = document.createElement("img");
newImg.src = "https://raw.githubusercontent.com/guowei1651/Habit-Formation-Auditorium/refs/heads/main/test/73b8a0bcf8c74d86a4c51bcff5814ebf.jpeg"; // 设置 img 的图片地址
newImg.style.opacity = "1"; // 设置图片的透明度
newImg.style.width = "100%"; // 设置 img 的宽度为 div 的100%
newImg.style.height = "100%"; // 设置 img 的高度为 div 的100%
newImg.style.borderRadius = "5px"; // 设置图片的圆角
newImg.style.objectFit = "scale-down"; // 使图片随 div 的大小变化

// 创建一个新的关闭按钮
const closeButton = document.createElement("button");
closeButton.textContent = "X"; // 设置按钮的文本为 "×"
closeButton.style.position = "absolute"; // 使关闭按钮的位置相对于 div 定位
closeButton.style.top = "5px"; // 使关闭按钮距离 div 顶部 5px
closeButton.style.right = "5px"; // 使关闭按钮距离 div 右侧 5px
closeButton.style.width = "30px"; // 设置按钮的宽度
closeButton.style.height = "30px"; // 设置按钮的高度
closeButton.style.padding = "0"; // 设置按钮的内边距
closeButton.style.border = "none"; // 设置按钮的边框
// closeButton.style.backgroundColor = "#ff4d4d"; // 设置按钮的背景颜色
closeButton.style.color = "white"; // 设置按钮的文字颜色
closeButton.style.cursor = "pointer"; // 设置按钮的鼠标指针样式
closeButton.style.borderRadius = "50%"; // 设置按钮的圆角
closeButton.onclick = function() { 
  newDiv.style.display = "none"; 
};

// 将 img 元素和关闭按钮添加到 div 元素中
innerDiv.appendChild(newImg);
innerDiv.appendChild(closeButton);

newDiv.appendChild(innerDiv);
document.body.appendChild(newDiv);

// 显示逻辑
(async function () {
  console.log("content-script.js View start");

  // 测试 browser 对象是否可用
  if (typeof browser !== "undefined") {
    console.log("显示逻辑 Browser API is available in content script.");
  } else {
    console.error("显示逻辑 Browser API is undefined in content script.");
  }

  async function getCarouseSize() {
    return new Promise((resolve) => {
      browser.storage.local.get("carouseSize", (result) => {
        resolve(result.carouseItems || { "width": "417px", "height": "314px" });
      });
    });
  }

  var self = this;
  self.browser = browser;
  const divResizeObserver = new ResizeObserver(entries => {
    if (entries.length === 0) {
      return;
    }
    entry = entries[0];
    const width = Math.floor(entry.contentRect.width);
    const height = Math.floor(entry.contentRect.height);
    if (width === 0 || height === 0) {
      return;
    }
    var carouseSize = { "width": width, "height": height };
    console.log("ResizeObserver save size to local storage start", carouseSize);
    self.browser.storage.local.set({ carouseSize });
    console.log("ResizeObserver save size to local storage end", carouseSize);
  });
  divResizeObserver.observe(newDiv);

  const lastCarouseSize = getCarouseSize();
  console.log("lastCarouseSize", lastCarouseSize);
  newDiv.style.width = lastCarouseSize.width;
  newDiv.style.height = lastCarouseSize.height;

  console.log("content-script.js View end");
})();

// 轮询逻辑
(function () {
  console.log("content-script.js Carouse start");

  // 测试 browser 对象是否可用
  if (typeof browser !== "undefined") {
    console.log("轮询逻辑 Browser API is available in content script.");
  } else {
    console.error("轮询逻辑 Browser API is undefined in content script.");
  }
  this.browser = browser;
  var self = this;
  console.log("content-script.js Carouse ", this.browser, browser, self);
  
  async function getCarouseItems() {
    return new Promise((resolve) => {
      browser.storage.local.get("carouseItems", (result) => {
        resolve(result.carouseItems || []); // 默认轮询标识
      });
    });
  }

  var loadImageMethodIndex = 3;
  async function loadImage() {
      var imageUrl = carouseItems[index].chartUrl;
      newImg.src = `${imageUrl}`;
  }

  // 监听来自 background.js 的消息
  browser.runtime.onMessage.addListener((message) => {
    if (message.type === "TAB_ACTIVATED") {
      console.log("Message received from background:", message);
      
      newDiv.getBoundingClientRect().width;
      newDiv.getBoundingClientRect().height;

      const width = newDiv.getBoundingClientRect().width;

      const angle = Math.floor(width / 360 * 100);
      const gradient = `linear-gradient(${ angle }deg, rgba(0,143,104,1) 50%, rgba(250,224,66,1) 50%)`;

      entry.target.style.background = gradient;

      newImg.src = message.imageUrl;
    }
  });

  var index = 0;
  var carouseItems = [];
  async function showCarousel() {
    carouseItems = await getCarouseItems()
    console.debug("showCarousel", carouseItems);
    if (carouseItems.length === 0) {
      setTimeout(async () => await showCarousel(), 1000 * 5);
      console.log("showCarousel", "carouseItems is empty");
      return;
    }

    if (index >= carouseItems.length) {
      index = 0;
    }

    loadImage();
    setTimeout(async () => await showCarousel(), carouseItems[index].duration * 1000);
    index++;
  }
  // 显示轮询中的信息
  showCarousel();

  console.log("content-script.js Carouse end");
})();

console.log("content-script.js end");