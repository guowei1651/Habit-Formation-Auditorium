function listenForClicks() {
  document.addEventListener("click", (e) => {
    console.log("setCarouselIdButtonClick", e);
    if (e.target.tagName !== "BUTTON") {
      // Ignore when click is not on a button within <div id="popup-content">.
      return;
    }
    if (e.target.id !== "carouselIdButton") {
      // Ignore when click is not on a id="carouselIdButton" button.
      return;
    }

    const carouselId = document.getElementById("carouselId").value;
    const carouselIdNumber = Number(carouselId);
    if (!isNaN(carouselIdNumber) && carouselIdNumber > 0) {
      browser.storage.local.set({ carouselId });
      window.close();
    } else {
      alert("请输入一个大于0的数字！");
    }
  })
}

browser.tabs.executeScript({
  code: `console.log("executeScript");`
}).then(listenForClicks);