const btn = document.getElementById("submitBtn");
const resultText = document.getElementById("result");

btn.addEventListener("click", async () => {
    const userVal = document.getElementById("username").value;
    const passVal = document.getElementById("password").value;

    const response = await fetch("/login", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: `user=${userVal}&pass=${passVal}`
    });

    const serverMessage = await response.text();
    resultText.innerText = `Status Code: ${response.status} \n ${serverMessage}`;
});