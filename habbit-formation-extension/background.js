const browser = window.browser || window.chrome; // 兼容不同浏览器

const BASE_API_URL = "http://172.25.1.22:8080/"; // 替换为你的服务URL

browser.runtime.onInstalled.addListener(() => {
  console.log("Extension installed");

  // 从存储中获取轮播标识（或从其他地方获取）
  async function getCarouselId() {
    return new Promise((resolve) => {
      browser.storage.local.get("carouselId", (result) => {
        resolve(result.carouselId || "1"); // 默认轮询标识
      });
    });
  }

  async function updateCarousel() {
    const carouselId = await getCarouselId();
    // 请求时传递用户标识作为参数
    const response = await fetch(BASE_API_URL+`carousel/${carouselId}/carouselItems`);
    if (!response.ok) {
      throw new Error("Failed to fetch wallpaper");
    }
  
    const data = await response.json();
    if (data.code != 0) {
      throw new Error("Failed to fetch wallpaper. " + response.message);
    }
    console.log("carouseItems data:", data.data);
    carouseItems = data.data;
    browser.storage.local.set({ carouseItems });
    console.log("carouseItems carouseItems2:", carouseItems);
  }

  // 定时更新轮询中的信息
  updateCarousel()
  setInterval(async ()=>await updateCarousel(), 1000*60*5);
});
