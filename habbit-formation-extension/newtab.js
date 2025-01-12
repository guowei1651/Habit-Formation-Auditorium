// https://developer.mozilla.org/zh-CN/docs/Mozilla/Add-ons/WebExtensions

async function getCarouseItems() {
  return new Promise((resolve) => {
    browser.storage.local.get("carouseItems", (result) => {
      resolve(result.carouseItems || []); // 默认轮询标识
    });
  });
}

var index = 0;
var carouseItems = [];
async function showCarousel() {
  carouseItems = await getCarouseItems()
  console.log("showCarousel", carouseItems);
  if (carouseItems.length === 0) {
    setTimeout(async () => await showCarousel(), 1000 * 5);
    console.log("showCarousel", "carouseItems is empty");
    return;
  }

  if (index >= carouseItems.length) {
    index = 0;
  }

  setTimeout(async () => await showCarousel(), carouseItems[index].duration * 1000);

  if (false) {
    var imageUrl = carouseItems[index].chartUrl;
    document.body.style.backgroundImage = `url(${imageUrl})`;
  } else {
    fetch('http://172.25.1.22:9001/7260742d-4557-441e-b443-6063da743562')
      .then(response => response.blob())
      .then(blob => {
        const ImageBlogurl = URL.createObjectURL(blob);
        document.body.style.backgroundImage = `url(${ImageBlogurl})`;
        URL.revokeObjectURL(ImageBlogurl); // 释放内存
      }).catch(error => console.error('Error:', error));
  }
  index++;
}

// 获取壁纸并设置背景
async function fetchWallpaper() {
  try {
    // 显示轮询中的信息
    showCarousel(carouseItems);
  } catch (error) {
    console.error("Error fetching wallpaper:", error);
  }
}

// 初始化
fetchWallpaper();
