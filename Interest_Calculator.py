p = float(input("Principal: $"))
c = (12 * float(input("Monthly Contribution: $")))
i = (.01 * float(input("Interest Rate: %")))
cmp = int(input("Compounded how many times annually? "))
age = int(input("Current Age: "))
retire = int(input("How old do you want to be when you retire? "))

year = 1

while age <= retire:
    p = p * ((1 + (i / cmp)) ** cmp)
    p += c
    age += 1
    year += 1
    print(f'Year: {year:<3}| Balance: ${p:.2f} | Age: {age}')