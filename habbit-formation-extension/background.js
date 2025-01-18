/*
后台服务

功能：
1. 轮询查询轮播的内容，以方便进行触发时间计算（每五分钟查询一次）
2. 处理当前页问题
*/

const browser = window.browser || window.chrome; // 兼容不同浏览器

const BASE_API_URL = "http://172.25.1.22:8090/"; // TODO：申请域名后替换域名

// 从存储中获取轮播标识（或从其他地方获取）
async function getCarouselId() {
  return new Promise((resolve) => {
    browser.storage.local.get("carouselId", (result) => {
      resolve(result.carouselId || "0"); // 默认轮询标识
    });
  });
}

// 从存储中获取用户登录信息
async function getUserLoginInfo() {
  return new Promise((resolve) => {
    browser.storage.local.get("user", (result) => {
      resolve(result.user || null); // 默认轮询标识
    });
  });
}

// cron触发轮播进行
var currentTab = {}
async function cronTriggerCarousel(carouseItem) {
  console.log("cronTriggerCarousel : ", new Date(), currentTab, carouseItem)
  let fields = Object.keys(currentTab)
  if (fields && fields.length > 0) {
    // 向当前标签页的 content-script.js 发送消息
    browser.tabs.sendMessage(currentTab.id, {
      type: "TAB_ACTIVATED",
      content: carouseItem,
    }).catch((err) => {
      console.error("Failed to send message to content script:", err);
    });
  } else {
    // 防止没有当前tab的时候，无法通知问题
    setTimeout(() => cronTriggerCarousel(carouseItem), 1000);
  }
}

var jobs = []
function scheduleRemindCarouseItem(carouseItem) {
  console.log("scheduleRemindCarouseItem param:", carouseItem)
  const job = new Cron(carouseItem.trigger_time, () => {
    console.log('提醒的调度开始');
    cronTriggerCarousel(carouseItem)
    console.log('提醒的调度结束');
  });
  jobs.push(job)
}

function scheduleHabitCarouseItem(carouseItem) {
  console.log("scheduleHabitCarouseItem param:", carouseItem)
  const job = new Cron(carouseItem.trigger_time, () => {
    console.log('习惯的调度开始');
    // TODO：需要获取习惯的计数或者别的区分与提醒的事项
    cronTriggerCarousel(carouseItem)
    console.log('习惯的调度结束');
  });
  jobs.push(job)
}

function scheduleLongScheduleCarouseItem(carouseItem) {
  console.log("scheduleLongScheduleCarouseItem param:", carouseItem)
  const job = new Cron(carouseItem.trigger_time, () => {
    console.log('长日程的调度开始');
    // TODO：需要计算当前长日程内容
    cronTriggerCarousel(carouseItem)
    console.log('长日程的调度结束');
  });
  jobs.push(job)
}

var currentCarouseItem = {}
function scheduleCarouseItem(carouseItem) {
  if (!carouseItem && !currentCarouseItem) {
    console.log("调度轮播项失败，因为参数为空")
    return
  }
  if (carouseItem) {
    currentCarouseItem = carouseItem
  } else {
    carouseItem = currentCarouseItem
  }
  // 轮播项的类型。1：代表美图，2：代表提醒，3：代表习惯，4：代表长日程
  if (carouseItem.type == 2) {
    scheduleRemindCarouseItem(carouseItem)
  }
  if (carouseItem.type == 3) {
    scheduleHabitCarouseItem(carouseItem)
  }
  if (carouseItem.type == 4) {
    scheduleLongScheduleCarouseItem(carouseItem)
  }
}

function cleanJobs() {
  if (jobs) {
    for (let index = 0; index < jobs.length; index++) {
      const job = jobs[index];
      job.stop()
    }
  }

  jobs = []
}

function startJobs(carouseItems) {
  if (!carouseItems) {
    return
  }
  for (let index = 0; index < carouseItems.length; index++) {
    const element = carouseItems[index];
    scheduleCarouseItem(element)
  }
}

async function updateCarousel() {
  const carouselId = await getCarouselId();
  if (!carouselId) {
    console.log("还没有选择轮询")
    return;
  }
  const userInfo = await getUserLoginInfo();
  if (!userInfo || !userInfo.id) {
    console.log("还没有登录")
    return;
  }

  // 请求时传递用户标识作为参数
  const response = await fetch(BASE_API_URL+`carousels/${carouselId}/items`,{
    method: "GET",
    headers: {
      "HF-User-Id": userInfo.id,
      "Content-Type": "application/json"
    }});
  if (!response.ok) {
    throw new Error("Failed to fetch carousel items. " + response);
  }

  const data = await response.json();
  if (data.code != 0) {
    throw new Error("Failed to fetch carousel items. " + data.message);
  }
  console.log("carouseItems data:", data.data);
  carouseItems = data.data;

  // 删除原来的job
  cleanJobs()

  // 启动现在的调度
  startJobs(carouseItems)

  browser.storage.local.set({ carouseItems });
  console.log("carouseItems carouseItems2:", carouseItems);
}

async function habitRecord(carouseItem, sendResponse) {
  console.log("习惯计数", carouseItem)
  const userInfo = await getUserLoginInfo();
  if (!userInfo || !userInfo.id) {
    console.log("还没有登录")
    return;
  }

  let habitId = carouseItem.relations_id
  fetch(BASE_API_URL+`habits/${habitId}/record`, {
    method: "POST",
    headers: {
      "HF-User-Id": userInfo.id,
      "Content-Type": "application/json"
    },
    body: JSON.stringify({ "type": Number(carouseItem.type), "serial" : "browser",  "remark": "" })
  }).then(response => {
    console.log("习惯记录结果:"+JSON.stringify(response))
    if (!response.ok) {
      sendResponse(response)
      console.log("记录失败:"+JSON.stringify(response))
    }
  }).catch(error => {
    console.error(error);
    sendResponse(error)
  });
}

// 监听各处来的消息
browser.runtime.onMessage.addListener((message, sender, sendResponse) => {
  // 处理选择carouse事件
  if (message.action === "SELECTED_CAROUSEL") {
    updateCarousel()
  } else if (message.action === "HABIT_RECORD") {
    habitRecord(message.data.carouseItem, sendResponse)
  }
});

// 监听用户切换标签页的事件
browser.tabs.onActivated.addListener(async (activeInfo) => {
  try {
    const tab = await browser.tabs.get(activeInfo.tabId);
    currentTab = tab
    if (tab.url) {
      console.log("Switched to tab:", tab.url);
    }
    scheduleCarouseItem()
  } catch (error) {
    console.error("Error handling tab activation:", error);
  }
});

// 监听插件安装事件
browser.runtime.onInstalled.addListener(() => {
  console.log("Extension installed");
});

// 定时更新轮询中的信息
updateCarousel()
setInterval(async ()=>await updateCarousel(), 1000*60*5);