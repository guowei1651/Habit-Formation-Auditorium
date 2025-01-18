// https://developer.mozilla.org/zh-CN/docs/Mozilla/Add-ons/WebExtensions

/*
内容脚本，主要功能为：插入提示内容到当前页

依赖功能：
1. 图床
2. 最基本的习惯配置

其中包括细化功能：
0. 提醒方式：所有页面展示？当前页面展示？
1. 触发时间计算
2. 根据位置配置进行位置展示
3. 记录大小，下次展示时使用该大小
4. 习惯的计数
5. 关闭按钮
*/
console.log("content-script.js start");

// 测试 browser 对象是否可用
if (typeof browser !== "undefined") {
  console.log("Browser API is available in content script.");
} else {
  console.error("Browser API is undefined in content script.");
}

async function getCarouseItems() {
  return new Promise((resolve) => {
    browser.storage.local.get("carouseItems", (result) => {
      resolve(result.carouseItems || []); // 默认轮询标识
    });
  });
}

async function getCarouseSize() {
  return new Promise((resolve) => {
    browser.storage.local.get("carouseSize", (result) => {
      resolve(result.carouseItems || { "width": "417px", "height": "314px" });
    });
  });
}

async function typeSpecificClean() {
  // 轮播项的类型。1：代表美图，2：代表提醒，3：代表习惯，4：代表长日程
  hideButton()
}

async function typeSpecificProcessing(carouseItem) {
  // 轮播项的类型。1：代表美图，2：代表提醒，3：代表习惯，4：代表长日程
  console.log("typeSpecificProcessing",carouseItem)
  console.log("typeSpecificProcessing 判断结果: ", carouseItem.type == 3)
  if (carouseItem.type == 3) {
    // TODO：验证这部分代码是否正确
    showCounterButton(carouseItem, async () => {
      browser.runtime.sendMessage({ action: "HABIT_RECORD", data: { carouseItem } })
        .then((data) => {
          alert("记录失败:"+JSON.stringify(error))
        });
    });
  }
}

// 显示逻辑
async function viewLogic() {
  console.log("content-script.js View start");

  // 测试 browser 对象是否可用
  if (typeof browser !== "undefined") {
    console.log("显示逻辑 Browser API is available in content script.");
  } else {
    console.error("显示逻辑 Browser API is undefined in content script.");
  }

  var newDiv = document.getElementById(divId)

  // 记录提示框大小的代码块
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

    const angle = Math.floor(width / 360 * 100);
    const gradient = `linear-gradient(${ angle }deg, rgba(0,143,104,1) 50%, rgba(250,224,66,1) 50%)`;
    newDiv.style.background = gradient;

  });
  divResizeObserver.observe(newDiv);

  const lastCarouseSize = getCarouseSize();
  console.log("lastCarouseSize", lastCarouseSize);
  newDiv.style.width = lastCarouseSize.width;
  newDiv.style.height = lastCarouseSize.height;

  console.log("content-script.js View end");
};

// 轮询逻辑
async function carouseLogic() {
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

  // TODO：这里需要判断是不是需要展示
  // （现方案）方案一：在展示某个Item之后进行轮播中所有的Item的《轮播》展示。
  var index = 0;
  async function loadImage() {
    var imageUrl = carouseItems[index].chartUrl;
    var newImg = document.getElementById("hfCarouseItemDisplayImage")
    newImg.src = `${imageUrl}`;
  }

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
}

// 监听来自 background.js 的消息
browser.runtime.onMessage.addListener((message) => {
  if (message.type === "TAB_ACTIVATED") {
    if (checkDivExist()) {
      console.log("已经有了div")
      typeSpecificClean()
      showElement()
      setImageSrc(message.content.chartUrl)
      typeSpecificProcessing(message.content)
      return
    }

    console.log("step 1",  message);
    insertElement()
    console.log("step 2",  message);
    typeSpecificProcessing(message.content)
    console.log("step 3",  message);
    viewLogic()
    console.log("step 4",  message);
    carouseLogic()
    console.log("step 5",  message);
  }
});

console.log("content-script.js end");