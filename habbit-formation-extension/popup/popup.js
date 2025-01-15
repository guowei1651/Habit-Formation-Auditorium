const BASE_API_URL = "http://172.25.1.22:8080/"; // 替换为你的服务URL
const BASE_GROWING_URL = "http://172.25.1.22/"

function login() {
  var username = document.getElementById('username').value;
  var password = document.getElementById('password').value;
  if (!username || !password) {
    document.getElementById("error").style.display = "block";
    document.getElementById("error").textContent = "发生错误，请稍后重试";
    return;
  }

  // 登录逻辑
  fetch(BASE_API_URL + "login", {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify({ username, password })
  }).then(response => {
    if (!response.ok) {
      document.getElementById("error").style.display = "block";
      document.getElementById("error").textContent = "发生错误，请稍后重试";
    }
    return response.json();
  }).then(data => {
    if (data.code !== 0) {
      $('#error').show().text('用户名或密码错误');
    } else {
      // 如果登录成功，显示组播选择内容
      document.getElementById("error").style.display = "none";
      document.getElementById("loginForm").style.display = "none";
      document.getElementById("tabContainer").style.display = "block";
      browser.storage.local.set({ 'login': data });
    }
  }).catch(error => {
    console.error(error);
    document.getElementById("error").style.display = "block";
    document.getElementById("error").textContent = error.message;
  });
}

// 从存储中获取轮播标识（或从其他地方获取）
async function getUserLoginInfo() {
  return new Promise((resolve) => {
    browser.storage.local.get("login", (result) => {
      resolve(result | null); // 默认轮询标识
    });
  });
}

async function loadCarousel() {
  const userInfo = await getUserLoginInfo();
  if (!userInfo) {
    alert("请先登录！");
    return;
  }

  // 请求时传递用户标识作为参数
  const response = await fetch(BASE_API_URL + `carousel?userId=${userInfo.id}`);
  if (!response.ok) {
    throw new Error("Failed to fetch carousel");
    return;
  }

  const data = await response.json();
  if (data.code != 0) {
    throw new Error("Failed to fetch carousel. " + response.message);
  }
  console.log("carouses data:", data.data);
  carouses = data.data;
  document.getElementById("carouselTable").innerHTML = carouses.map((item, index) => {
    return `
      <tr>
        <td>${item.id}</td>
        <td>${item.name}</td>
        <td><button id="carouses${item.id}" class="btn btn-primary btn-block carouses-marker" onclick="chooseCarousel(${item.id})">选择</td>
      </tr>
    `;
  })

  document.getElementById(`carouses${item.id}`).disabled = true;
  document.getElementById(`carouses${item.id}`).btn.classList.add('selected');
  document.getElementById(`carouses${item.id}`).textContent = "已选择";

  console.log("carouseItems carouseItems2:", carouseItems);
}

function chooseCarousel(carouselId) {
  console.log("setCarouselIdButtonClick", e);

  const carouselIdNumber = Number(carouselId);
  browser.storage.local.set({ carouselId });

  document.querySelectorAll('.carouses-marker').forEach(btn => {
    btn.classList.remove('selected');
    btn.textContent = "选择";
    btn.disabled = false;
  }).then(() => {
    document.getElementById(`carouses${carouselId}`).disabled = true;
    document.getElementById(`carouses${carouselId}`).btn.classList.add('selected');
    document.getElementById(`carouses${carouselId}`).textContent = "已选择";
  });
}

function choosePosition(marker) {
  return function () {
    // 清除其他按钮的选中状态
    document.querySelectorAll('.carouses-marker').forEach(btn => btn.classList.remove('selected'));
    // 当前按钮添加选中状态
    marker.classList.add('selected');
  }
}

function listenForClicks() {
  // 忘记密码链接
  document.getElementById("forgetPassword").href = BASE_GROWING_URL + "forgetPassword";

  // 登录按钮
  document.getElementById('loginButton').addEventListener('click', login);

  // 选择轮播标识按钮
  document.addEventListener("click", chooseCarousel)

  // 选择位置按钮
  document.querySelectorAll('.position-marker')
    .forEach(marker => marker.addEventListener('click', marker.addEventListener('click', choosePosition(marker))));
}

document.onload = listenForClicks;
window.onload = listenForClicks;
/*
browser.tabs.executeScript({
  code: `console.log("executeScript");`
}).then(listenForClicks);
*/