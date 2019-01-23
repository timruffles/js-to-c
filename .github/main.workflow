workflow "Build and verify" {
  on = "push"
  resolves = ["CI"]
}

action "CI" {
  uses = "./actions/ci"
}
