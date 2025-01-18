/*
浏览器插件弹出页面部分

功能：
1. 登录
2. 轮播选择
3. 展示位置选择
*/

const BASE_API_URL = "http://172.25.1.22:8090/"; // 替换为你的服务URL
const BASE_GROWING_URL = "http://172.25.1.22/"

// 登录操作
function login() {
  var username = document.getElementById('username').value;
  var password = document.getElementById('password').value;
  if (!username || !password) {
    document.getElementById("error").style.display = "block";
    document.getElementById("error").textContent = "发生错误，请稍后重试";
    return;
  }

  // 登录逻辑
  fetch(BASE_API_URL + "users/login", {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify({ username, password })
  }).then(response => {
    console.info("用户登录返回:", response)
    if (!response.ok) {
      document.getElementById("error").style.display = "block";
      document.getElementById("error").textContent = "发生错误，请稍后重试";
    }
    return response.json();
  }).then(data => {
    if (data.code !== 0) {
      console.error("用户登录失败:", data.message)
      document.getElementById("error").style.display = "block";
      document.getElementById("error").textContent = "用户名或密码错误";
    } else {
      // 如果登录成功，显示组播选择内容
      document.getElementById("error").style.display = "none";
      document.getElementById("loginForm").style.display = "none";
      document.getElementById("tabContainer").style.display = "block";
      var user = data.data
      browser.storage.local.set({ user })
      loadCarousel()
      loadPosition()
    }
  }).catch(error => {
    console.error(error);
    document.getElementById("error").style.display = "block";
    document.getElementById("error").textContent = error.message;
  });
}

// 从存储中获取用户登录信息
async function getUserLoginInfo() {
  return new Promise((resolve) => {
    browser.storage.local.get("user", (result) => {
      resolve(result.user || {});
    });
  });
}

// 从存储中获取轮播标识（或从其他地方获取）
async function getCarouselId() {
  return new Promise((resolve) => {
    browser.storage.local.get("carouselId", (result) => {
      resolve(result.carouselId || "0"); // 默认轮询标识
    });
  });
}

// 从存储中获取轮播的位置
async function getPosition() {
  return new Promise((resolve) => {
    browser.storage.local.get("position", (result) => {
      resolve(result.position || "bottom-right"); // 默认轮询标识
    });
  });
}

// 从服务端加载轮播信息
async function loadCarousel() {
  var userInfo = await getUserLoginInfo();
  console.log("loadCarousel userInfo->", JSON.stringify(userInfo))
  if (!userInfo) {
    console.log("请先登录！", userInfo)
    alert("请先登录！");
    return;
  }

  // 请求时传递用户标识作为参数
  const response = await fetch(BASE_API_URL + `carousels`,{
    method: "GET",
    headers: {
      "HF-User-Id": userInfo.id,
      "Content-Type": "application/json"
    }
  });
  if (!response.ok) {
    throw new Error("Failed to fetch carousel");
    return;
  }

  const data = await response.json();
  if (data.code != 0) {
    throw new Error("Failed to fetch carousel. " + response.message);
  }
  carouses = data.data;
  console.log("carouses data:", JSON.stringify(carouses));
  document.getElementById("carouselTable").innerHTML = carouses.map((item, index) => {
    return `
      <tr>
        <td>${item.id}</td>
        <td>${item.name}</td>
        <td><button id="carouses${item.id}" class="btn btn-primary btn-block carouses-marker">选择</td>
      </tr>
    `;
  })

  // 设置轮播的按钮的处理函数
  carouses.map((item, index) => {
    const buttonId = `carouses${item.id}`
    console.log("current carouse id : ", buttonId)
    document.getElementById(buttonId).onclick = function () {
      return chooseCarousel(item.id)
    };
  })

  // 设置当前选择的轮播
  var currentCarouselId = await getCarouselId()
  if (currentCarouselId != 0) {
    const buttonId = `carouses${currentCarouselId}`
    console.log("current carouse id : ", buttonId)
    document.getElementById(buttonId).disabled = true;
    document.getElementById(buttonId).classList.add('selected');
    document.getElementById(buttonId).textContent = "已选择";
  }

  console.log("carouses carouses:", carouses);
}

// 用户选择轮播
function chooseCarousel(carouselId) {
  console.log("chooseCarousel", carouselId);

  const carouselIdNumber = Number(carouselId);
  browser.storage.local.set({ carouselId });

  var elements = document.querySelectorAll('.carouses-marker')
  if (! elements) {
    console.log("未找到相关class的标签")
    return
  }
  console.log("找到相关class的标签, elements:", elements)
  
  for (let index = 0; index < elements.length; index++) {
    const btn = elements[index];
    btn.classList.remove('selected');
    btn.textContent = "选择";
    btn.disabled = false;
  }

  var elementId = `carouses${carouselId}`
  console.log("element id:", elementId)
  var selectedBtn = document.getElementById(elementId)
  selectedBtn.disabled = true;
  selectedBtn.classList.add('selected');
  selectedBtn.textContent = "已选择";

  // 向background.js发送消息，通知用户选择的轮询
  browser.runtime.sendMessage({ action: "SELECTED_CAROUSEL", data: {carouselId} });
}

async function loadPosition() {
  var position = await getPosition()
  // 清除其他按钮的选中状态
  document.querySelectorAll('.position-marker').forEach(btn => btn.classList.remove('selected'));
  // 当前按钮添加选中状态
  document.getElementById(position).classList.add('selected')
}

// 用户选择提醒位置
function choosePosition(marker) {
  return function () {
    // 清除其他按钮的选中状态
    document.querySelectorAll('.position-marker').forEach(btn => {
      btn.classList.remove('selected')
    });
    // 当前按钮添加选中状态
    marker.classList.add('selected');

    // 查看当前标签
    console.log("choosePosition marker->", marker)
    var position = marker.id
    browser.storage.local.set({ position })
  }
}

// 监听所有的Click动作
async function listenForClicks() {
  // 检查已经登录则跳转到轮播页
  console.log("检查已经登录则跳转到轮播页")
  const userInfo = await getUserLoginInfo();
  console.log("listenForClicks userInfo:", JSON.stringify(userInfo))

  if (userInfo && userInfo.id && userInfo.username && userInfo.email) {
    console.log("登录过之后显示轮播数据")
    document.getElementById("error").style.display = "none";
    document.getElementById("loginForm").style.display = "none";
    document.getElementById("tabContainer").style.display = "block";
    loadCarousel()
    loadPosition()
  }

  // 忘记密码链接
  document.getElementById("forgetPassword").href = BASE_GROWING_URL + "forgetPassword";

  // 登录按钮
  document.getElementById('loginButton').addEventListener('click', login);

  // 选择位置按钮
  document.querySelectorAll('.position-marker')
    .forEach(marker => marker.addEventListener('click', marker.addEventListener('click', choosePosition(marker))));
}

listenForClicks()