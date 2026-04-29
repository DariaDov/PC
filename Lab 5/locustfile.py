from locust import HttpUser, task

class HelloWorldUser(HttpUser):
    @task
    def load_index(self):
        self.client.get("/index.html")

    @task
    def load_page(self):
        self.client.get("/page2.html")
